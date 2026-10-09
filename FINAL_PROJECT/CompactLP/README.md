# OpenEPT — ESP8266 - STM32

Firmware middleware je sloj koji povezuje STM32 energetski profiler sa Qt GUI aplikacijom preko WiFi-ja. Ovaj dokument je **referenca svih komandi** koje postoje u sistemu.

## Komponente

- **STM32 - Nucleo-L476RG** — baziran na FreeRTOS-u. Arhitektura podeljena na 3 glavna taska: 

- **ESP8266** — mrežni most: prima linije sa STM32 preko UART-a, prosleđuje ih GUI-ju preko TCP-ja  i UDP-a, podiže WiFi u odredjenom modu izvršavanja(STA/AP/AP+STA), i odrzava web log stranicu.
- **GUI** — šalje konfiguracione komande i prima podatke od uređaja(ESP).


## Arhitektura STM32 Firmware-a (FreeRTOS)
**Taskovi:**
 - SensorTask - neprekidno očitavanje podataka sa senzora (za sada random vrednosti), pakuje podatke u binarne pakete sa `0xAA` zaglavljem radi bržeg prenosa.
 - ParserTask - prihvata poruke preko UART-a, parsira ih i prosledjuje dalje 
 - UartTxTask - task za bezbedno slanje svih poruka (tekstualnih i binarnih) ka ESP-u preko UART-a

## Arhitektura toka podataka

```
Senzor <--I2C--> STM32 <--UART--> ESP8266 <--WiFi--> GUI (TCP/UDP)

```

- **GUI <-> ESP**  : TCP passthrough (konfiguracione komande i odgovori) + UDP (stream podaci)
- **STM32 <-> ESP**: ova UART komunikacija pravljena je za dve različite namene:
  1. (**PRAKSA** deo) STM32 šalje svoje poruke sa prefiksom (`info:`, `echo:`, `check:`) ESP-u
  2. (**DIPLOMSKI** deo) STM32 šalje preko ESP-a grafičkom interfejsu GUI-u različite odgovore (`OK [msg]`) ili podatke očitane sa senzora (random vrednosti za sada)
- **ESP**: `CommandParser::parseMessage()` gleda prefiks (case-insensitive) i odlučuje šta da radi; `CmdDispatcher` izvršava akciju (log na web stranicu, restart servera, forward ka GUI-ju preko TCP-a...).

---

## 1. Mrežna konfiguracija (STA / AP / AP+STA)

ESP ne prima wifi mod direktno sa GUI-ja — režim se šalje **sa STM32 na ESP** kao `info:` poruka. Na PC-ju se to pokreće `transmit` komandom od strane STM32.

### Predefinisane komande(`Transfer_Handle`, `transfer_control.c`):

| PC komanda | Šta radi | Poruka koju STM32 šalje ESP-u |
|---|---|---|
| `transmit infomsg1` | STA mod (ESP se konektuje na ruter) | `info: mode=STA;tcp_port=13450;http_port=80;ssid=HUAWEI_B535_11C6;pass=6aLhEDnaG2F;` |
| `transmit infomsg2` | AP mod (ESP postaje ruter) | `info: mode=AP;tcp_port=5000;http_port=8081;ssidAP=ESP_NET;passAP=23456789;ipAP=192.168.10.30;` |
| `transmit infomsg3` | AP+STA mod (i jedno i drugo) | `info: mode=AP+STA;tcp_port=3000;http_port=8082;ssid=HUAWEI_B535_11C6;pass=6aLhEDnaG2F;ssidAP=ESP_NET;passAP=12345678;ipAP=192.168.50.5;` |

### Ručno unošenje komandi preko `transmit value=`:

`transmit value=` prosleđuje tekst direktno na ESP UART, pa ako taj tekst počinje sa `info:` i sadrži prepoznate ključeve, `ServerParamsParser` ga parsira:

```
transmit value="info: mode=AP;ssidAP=MojaMreza;passAP=lozinka123;ipAP=192.168.4.1;tcp_port=5000;http_port=8081;"
```

**Validni ključevi** (`ServerParamsParser::apply`):

| Ključ | Značenje | Default ako izostavljen |
|---|---|---|
| `mode` | `STA` \| `AP` \| `AP+STA` | `STA` |
| `ssid` | SSID rutera na koji se ESP konektuje (STA) | `definisano u Common/constants.h` |
| `pass` | lozinka za taj ruter | `definisano u Common/Constants.h` |
| `ssidAP` | SSID koji ESP emituje (AP) | `definisano u Common/Constants.h` |
| `passAP` | lozinka za AP koji ESP emituje | `definisano u Common/Constants.h` |
| `ipAP` | IP adresa ESP-a u AP modu | `definisano u Common/Constants.h` |
| `http_port` | port za web log stranicu | `80` |
| `tcp_port` | port za GUI↔STM32 passthrough | `8080` |

Format parova: `key=value;key2=value2;...` (Jedan par je razdvojen sa `;`).

`mode=STA` ili `mode=AP+STA` znači ESP se konektuje kao klijent na ruter. `mode=AP` znači ESP sam postaje rute na koji se kačimo. `AP+STA` radi oboje istovremeno.

---

## 2. STM32 Command Set (Set komandi)

Sve komande koje stižu na STM32 prihvata `ParserTask`, parsira ih u fajlu `command_parser.c`, a zatim rutira dalje na izvršavanje. Komande su podeljene u tri glavne kategorije zavisno od njihove namene.

### 2.1 Lokalna kontrola uređaja (`show`) — PC <-> STM32
Ovaj set komandi je napravljen u okviru **PRAKSA** dela projekta.

Ova komanda je u početku sadržala `device` umesto `show` ali je zbog upotrebe pomenute reči u komandama za komunikaciju sa GUI-em napravljena izmena.

Ove komande se obrađuju direktno u modulu `device_control.c` (u `Platform` folderu) i služe za upravljanje hardverom na samoj Nucleo pločici.

| Komanda | Efekat |
|---|---|
| `show diode on` | Uključuje LED diodu na STM32 |
| `show diode off` | Isključuje LED diodu na STM32 |
| `show diode toggle` | Menja stanje LED diode (Toggle) |
| `show status diode` | Vraća odgovor: "LED is ON." ili "LED is OFF." |
| `show status button` | Vraća odgovor: "BUTTON is PRESSED." ili "BUTTON is NOT PRESSED." |

***Napomena**: Ako se pošalje `transmit behavior show ...`, komanda se ne izvršava na STM32, već se prosleđuje ESP-u da on kontroliše svoje periferije (LED/button).*

### 2.2 Komunikacija sa mrežnim mostom (`transmit`) — PC -> STM32 -> ESP
Ovaj set komandi je napravljen u okviru **PRAKSA** dela projekta.

Služi za ručno slanje poruka i prekonfiguraciju ESP modula preko UART-a (salje se preko `UartTxTask`).

| Komanda | Efekat |
|---|---|
| `transmit check` | Generiše test podatke i šalje ih ESP-u u precizno definisanom formatu (Pritisak tastera na ploči je prečica za slanje istih podataka) |
| `transmit value="tekst"` | Šalje proizvoljan tekstualni string direktno na ESP |
| `transmit behavior <tekst>` | Šalje argument na ESP (napravljeno za upravljanje periferijama ESP-a) |
| `transmit clear` | Šalje `clear` ESP-u za brisanje web log-a |
| `transmit infomsg1/2/3` | Prečica za slanje Wi-Fi konfiguracionih parametara (STA / AP / AP+STA) |

### 2.3 GUI Konfiguracija i Streamovi (`device`, `stream`, `charger`) — STM32 <-> ESP <-> GUI
Ovo je jezgro **DIPLOMSKOG** dela. Komande stižu sa GUI-ja (kroz ESP TCP most), a logika za njihovo parsiranje i formiranje odgovora nalazi se u modulu `configuration_control.c`. Za svaku od ovih komandi, STM32 obavezno vraća odgovor u formatu `OK [msg]` ili `ERROR 1`.

***Napomena**: Neke od komandi i dalje imaju template odgovore i većina nije uopšte testirana što će biti omogućeno tek kada budemo povezali senzor.*

#### Linkovi i streamovi (kreiranje)
| Komanda | Efekat |
|---|---|
| `device hello` | Resetuje sve streamove/linkove, vraća `OK <device_name>` |
| `device setname -name=X` | Postavlja ime uređaja |
| `stream create` | Alocira novi stream podataka, vraća `OK <sid>` |
| `stream start -sid=N` | Pokreće streaming podataka na streamu N (STM32 kreće da šalje binarne pakete) |
| `stream stop -sid=N` | Zaustavlja streaming |
| `eplink create` | Alocira endpoint link, vraća `OK <id>` |
| `slink create` | Alocira status link, vraća `OK <id>` |
| `slink send` | Šalje podatke preko status linka |

#### ADC parametri (stream-scoped, zahtevaju `-sid=N`)
| Komanda | Efekat (Podešavanje parametara akvizicije) |
|---|---|
| `device adc chresolution set/get -sid=N -value=X` | Rezolucija ADC kanala |
| `device adc chclkdiv set/get -sid=N -value=X` | Clock divider |
| `device adc chstime set/get -sid=N -value=X` | Sample time |
| `device adc chavrratio set/get -sid=N -value=X` | Averaging ratio (usrednjavanje) |
| `device adc speriod set/get -sid=N -period=X -prescaler=Y` | Sampling period (zahteva 2 parametra) |
| `device adc voffset set/get -sid=N -value=X` | Voltage offset |
| `device adc coffset set/get -sid=N -value=X` | Current offset |
| `device adc samplesno set -sid=N -value=X` | Broj uzoraka |
| `device adc clk get -sid=N` | (Placeholder) Uvek vraća `OK 0` |
| `device adc value get -sid=N` | (Placeholder) Uvek vraća `OK 0` |

#### DAC i prekidači (device-scoped, bez `-sid=`)
| Komanda | Efekat |
|---|---|
| `device dac enable set/get -value=X` | Uključi/isključi DAC |
| `device dac value set/get -value=X` | DAC izlazna vrednost |
| `device load enable/disable/get` | Prekidač za LOAD |
| `device bat enable/disable/get` | Prekidač za bateriju |
| `device ppath enable/disable/get` | Prekidač za Power Path |
| `device wave chunk add / state set / clear` | Wave generator (trenutno `OK OK`) |
| `device uvoltage/ovoltage/ocurrent get` | (Placeholder) Uvek vraća `OK 0` |
| `device latch trigger` | Okida latch |
| `device rgb setcolor` | Menja boju RGB LED-a |

#### Charger (potencijalno neće trebati)
| Komanda | Efekat |
|---|---|
| `charger charging enable/disable/get` | Uključi/isključi proces punjenja |
| `charger charging current set/get -value=X` | Podešava struju punjenja |
| `charger charging termcurrent set/get -value=X` | Podešava terminalnu struju |
| `charger charging termvoltage set/get -value=X` | Podešava terminalni napon (npr. `X.YY`) |
| `charger reg read` | (Placeholder) Vraća `OK OK: 0x00` |

***Napomena:** o parsiranju: Bilo koja komanda koja ne zadovolji `strstr()` proveru unutar `command_parser.c` biće odbačena. STM32 u tom slučaju ispisuje `Error: Else block discovered :(.` na lokalni PC terminal i šalje `ERROR 1` ka ESP-u).*

---

Ako GUI šalje sa prefiksom `device ` ispred (npr. `device stream create ...`), ESP (`TcpServer::stripDevice`) skida taj prefiks pre parsiranja i obradjuje link poruke.

## 3. ESP -> GUI / web log poruke (STM32 → ESP smer)

| Prefiks | Akcija na ESP-u |
|---|---|
| `error...` | forward GUI-ju (TCP) |
| `ok...` | forward GUI-ju (TCP) |
| `echo:<tekst>` | log na web stranicu (skida se `echo:`) |
| `check:[T=..;p=..;]` | log na web stranicu, oboji T/P prema pragovima |
| `info:key=val;...` | primeni na `ServerConfig`, restartuje HTTP/TCP servere |
| `clear` | očisti web log |
| `show...` | prosleđeno lokalnom `Device_Handle()` (LED/dugme na ESP-u) |
| bilo šta drugo | log kao `Invalid message: [...]` |

