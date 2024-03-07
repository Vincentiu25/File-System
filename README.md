Nume: Mazilu Flavius-Romeo & Tarsoaga Vincentiu-Ionut
Grupa: 314CA

# Tema2 - Simple Linux File System

## Organizare

* Main.c : meniul de comanda si aplarea functiilor\
* LinkedLIst.c/.h : Structurile si functiile de lista simplu inlantuita\
* tree.c/.h : Functiile implementare

## Descriere program

* Acest program reprezinta un sistem de fisiere si de comenzi specifice,
asemanator cu cel din Linux(cd, ls. mkdir, mv, pwd, rm etc). La inceputul
programului, utilizatorul se afla in root, fara sa existe niciun copil. In
urma comenzilor, se creeaza un sistem de fisiere care este construit pe 
baza listelor simplu inlantuite

## Implementare

* Implementarea programului se bazeaza pe ideea de lista in lista. Astfel, 
plecand din root, acesta are pointer catre o lista simplu inlantuita, care
reprezinta copiii acestuia. Fiecare nod din lista are un pointer catre
o structura de tip TreeNode, care reprezinta arborele incepand cu acel copil.
Fiecare nod din arbore are un nume, un tip si un pointer spre lista de copii.

* Un director este gol daca lista de copii are 0 noduri.

* Programul foloseste functiile standard de liste simplu inlantuite:
-Add_node : adaugarea unui nod in lista de copii(mkdir, touch)
-Remove_node : stergerea unui nod din lista de copii(rm, rmdir)
-Free_List : eliberarea unei liste de copii(rmrec)

* Este folosita recursivitatea pentru a parcurge arborele de la radacina la 
frunze

* Am creeat o functie find_name_in_folder pentru a gasi usor directorul sau 
fsierul asupra caruia dorim sa efectuam o comanda

* Inainte de a executa o comanda, verificam tipul nodului pentru a nu aplica
comenzi de folder la fisier si invers

## Precizari 

* Una dintre temele noastre preferate de pana acum cu siguranta. A fost o
experienta noua sa lucram in echipa pentru crearea unui program si consideram
ca a fost una benefica, deoarece ne-a invatat sa lucram in echipa. 

* A fost o tema placut de implementat deoarece, chiar daca initial parea
complicat de implementat, necesita in mare parte cunostinte despre liste 
simplu inlantuite, structuri si pointeri. Scheletul oferit a fost de asemenea
de ajutor.

* Suntem destul de multumiti de rezultatul final, dar consideram ca era 
posibila o modularizare mai buna a codului
