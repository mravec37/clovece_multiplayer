# Človeče, nehnevaj sa – sieťová hra (server / klient)

## Úvod

Našou úlohou v tejto semestrálnej práci bolo vytvoriť hru, ktorá beží na serveri a ku ktorej sa môžu pripojiť viacerí klienti. Hra mala správne fungovať s využitím **socketov** a **vlákien**.

Ako konkrétnu hru sme si zvolili **Človeče, nehnevaj sa**.  
V prvej časti dokumentácie je popísaná štruktúra projektu, použitie socketov a vlákien a riešený synchronizačný problém.  
V druhej časti dokumentácie je popísané, ako sa bežný užívateľ môže na server pripojiť, ako hru hrať a aké sú jej pravidlá.

---

## Programátorská dokumentácia

### Štruktúra projektu

Projekt pozostáva z **10 súborov typu `.c` a `.h`**.

- **server.c**  
  Obsahuje funkcionalitu servera. Po spustení programu s parametrami pre server  
  (napr. `server 13332`) sa vytvorí server, na ktorý sa môžu pripojiť **štyria klienti**.  
  Server následne komunikuje s klientmi a posiela im potrebné dáta.

- **client.c**  
  Obsahuje funkcionalitu klienta. Po spustení programu s parametrami  
  (napr. `client localhost 13332`) sa klient pripojí na server a prijíma informácie
  o stave hry a o tom, kedy môže zasiahnuť do hry.

- **game_logic.c**  
  Obsahuje funkcie súvisiace s logikou hry:
  - inicializácia a vykreslenie mapy
  - pohyb panáčikov
  - kontrola kolízií medzi panáčikmi

- **player.h**  
  Obsahuje `struct player`, ktorý reprezentuje hráča a obsahuje pole
  štyroch panáčikov typu `PIECE`.

- **piece.h**  
  Obsahuje `struct piece`, ktorý reprezentuje jedného panáčika:
  - `char sign` – textová reprezentácia panáčika
  - `int pos` – pozícia panáčika na mape

- **main.c**  
  Obsahuje funkciu `main`, ktorá podľa programových parametrov spustí
  buď server alebo klienta.

---

## Použitie socketov

Sockety boli použité na komunikáciu medzi serverom a klientmi, konkrétne
v súboroch **server.c** a **client.c**.

- Server vytvára **pasívny socket**, na ktorý sa pripájajú klienti.
- Pre každého klienta sa vytvorí nový socket určený na komunikáciu.
- Server čaká v cykle `while`, kým sa nepripoja všetci štyria klienti.
- Pre každého klienta sa vytvorí **samostatné vlákno**.

Komunikácia prebieha pomocou funkcií:
- `write`
- `read`

Ich návratové hodnoty sa používajú na zistenie:
- či sa klient neodpojil
- či nenastala chyba komunikácie

---

## Použitie vlákien

V súbore **server.c** sú vytvorené **4 vlákna typu `pthread_t`** – jedno pre každého hráča.

- Každé vlákno obsluhuje komunikáciu s jedným klientom
- Funkcia `clientHandler` sa spúšťa vo vláknach
- Do funkcie sa posiela aj **mutex**, ktorý zabraňuje nekorektnému prepisovaniu dát

Vlákna umožňujú serveru komunikovať s klientmi **paralelne**.

---

## Synchronizačný problém

Hlavným problémom bolo správne zosynchronizovať komunikáciu medzi serverom a klientmi.

Priebeh:
1. Server pošle hráčovi, ktorý je na rade, mapu hry
2. Klient mapu prijme a vypíše do konzoly
3. Server oznámi klientovi, že je na rade
4. Klient pošle serveru odpoveď (napr. hod kockou)
5. Server:
   - vygeneruje číslo 1–6
   - posunie panáčika
   - skontroluje kolízie
   - pošle aktualizovanú mapu klientom

Tento cyklus sa opakuje, kým jeden z hráčov nedostane všetkých panáčikov do domčeka.

---

## Kľúčové problémy

Medzi hlavné problémy patrili:
- implementácia herných mechaník
- testovanie a debugovanie (napr. miznutie panáčikov z mapy)
- počiatočná synchronizácia servera a klientov

Po vytvorení základnej komunikácie bolo pridávanie ďalších funkcionalít jednoduchšie.

---

## Používateľská dokumentácia

### Pripojenie

- Server beží na zadanom porte
- Pripojiť sa môžu **maximálne štyria hráči**
- Hra sa nezačne, kým nie sú pripojení všetci štyria hráči
- Po pripojení všetkých hráčov začne hra a na rade je prvý hráč

---

## Ovládanie a pravidlá hry

- Hráč, ktorý je na rade, je vyzvaný, či chce hádzať kockou
- Potvrdenie prebieha zadaním znaku **`y`**
- Systém vygeneruje číslo od **1 do 6**
- Každý hráč má **4 figúrky**
- Hráč vyberie figúrku zadaním čísla **1–4**

Herné pravidlá:
- Hracia plocha má **40 políčok**
- Domček pozostáva zo **4 políčok**
- Figúrky súperov je možné vyraziť späť na začiatok
- Ak hráč vstúpi na vlastnú figúrku, ťah sa stráca
- Ak hod presiahne možný pohyb do domčeka, ťah sa stráca

Hra pokračuje, kým jeden z hráčov nedostane všetky figúrky do domčeka.

---

## Ukončenie hry

Hra končí:
- víťazstvom jedného z hráčov
- alebo odpojením jedného z hráčov

O ukončení hry sú informovaní všetci hráči.

---

## Záver

V projekte sa nám podarilo vytvoriť funkčnú sieťovú hru, ktorá umožňuje
pripojenie štyroch klientov k serveru. Pomocou socketov a vlákien bola
zabezpečená korektná komunikácia medzi serverom a klientmi.

Hra funguje stabilne a správne rieši:
- priebeh ťahov
- synchronizáciu hráčov
- ukončenie hry
