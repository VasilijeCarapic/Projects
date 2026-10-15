def ucitaj_fajlove():
    in1 = input() 
    in2 = input()
    return in1, in2 

def ucitaj_podatke(in1, in2): 
    # id : naziv stanice 
    stanice = {}

    # id : (linija, redni_broj)
    linije = { }


    with open(in1, "r", encoding = "utf-8") as stanice_fajl, \
         open(in2, "r", encoding = "utf-8") as linije_fajl: 

        # parsiranje prvih podataka
        for l in stanice_fajl: 

            if l.strip() == "": continue 

            id, ime_stanice = l.strip().split("|")
            
            if id in stanice.keys(): 
                raise ValueError
            
            stanice[id] = ime_stanice    


        for l in linije_fajl: 
            if l.strip() == "": continue 

            linija, redni_broj, id = l.strip().split(",")
            try: 
                linije[id].append((linija, int(redni_broj)))
            except KeyError: 
                linije[id] = [(linija, int(redni_broj))]    

    return stanice, linije 





def procesiranje_podataka(stanice, linije):
    d = {}
    for id, spisak in linije.items():
        for linija, redni in spisak:          # raspakujemo tuple, nema [0][0]
            if linija not in d:
                d[linija] = []
            d[linija].append((redni, id, stanice[id]))

    for linija in d:
        d[linija].sort()                      # sortira po rednom broju

        # provera: redni brojevi moraju biti tačno 1, 2, ..., n
        brojevi = [redni for redni, id, ime in d[linija]]
        if brojevi != list(range(1, len(brojevi) + 1)):
            raise ValueError
    return d

def ispis_u_fajl(d):
    for k in d: 
        with open(k + ".txt", "w", encoding = "utf-8") as f: 
            for item in d[k]: 
                text = item[1] + " " + item[2] + "\n"
                f.write(text)

def main(): 
    try: 
        filename1, filename2 = ucitaj_fajlove()
        stanice, linije = ucitaj_podatke(filename1, filename2)
        d = procesiranje_podataka(stanice = stanice, linije = linije)
        ispis_u_fajl(d)

    except OSError:
        print("DAT_GRESKA")
    except Exception:
        print("GRESKA")


main()      

            