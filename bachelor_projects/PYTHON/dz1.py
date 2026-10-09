def ucitajDuzinuListe(): 
    n = int(input())
    return n


def ucitajNamirnice(n): 
    listaNamirnica = []

    ucitano = 0 
    while(ucitano < n): 
        l = input()
        
        if "|" not in l: 
            continue 
        
        l1, l2 = l.split("|")
        levo = l1.split(",")
        namirnice = [ float(levo[i]) if i >= 1 else levo[i] for i in range(len(levo))]
        namirnice.append(float(l2))
        listaNamirnica.append(namirnice)
        ucitano += 1 


    return listaNamirnica



def ucitajReferentneVrednosti(): 
    l = input().split()
    refVrednosti = [float(x) for x in l]
    return refVrednosti


def ukupnaKolicina(listaNamirnica, refVrednosti):
    d = {"Energija" : 0, "Masti" : 0, "Seceri" : 0, "Proteini" : 0, "Soli" : 0}
    n = len(listaNamirnica)
    kolicinaNa100g = [listaNamirnica[i][-1] / 100 for i in range(n)]

    for i in range(n): 
        kolicina = kolicinaNa100g[i]
        d["Energija"] += kolicina * listaNamirnica[i][1]
        d["Masti" ]   += kolicina * listaNamirnica[i][2]
        d["Seceri" ]  += kolicina * listaNamirnica[i][3]
        d["Proteini"] += kolicina * listaNamirnica[i][4]
        d["Soli"]     += kolicina * listaNamirnica[i][5]

    return d 

def obradaPodataka(d, refVrednosti): 
    procenti = {}
    i = 0 

    for k in d.keys(): 
        procenti[k] = (d[k] / refVrednosti[i]) * 100 
        i += 1 


    for k in d.keys(): 
        print("{} : {:0.2f} | {:0.2f}%".format(k, d[k], procenti[k]))
    

while(True):
    n = ucitajDuzinuListe()
    if(n < 0): break 

    namirnice = ucitajNamirnice(n)
    refVrednosti = ucitajReferentneVrednosti()
    d = ukupnaKolicina(namirnice, refVrednosti = refVrednosti)
    obradaPodataka(d, refVrednosti = refVrednosti)

