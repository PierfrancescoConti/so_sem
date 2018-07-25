# SO_SEM Project

Progetto universitario per l'esame di Sistemi Operativi con lo scopo di:

    Implementare una struttura elementare per la gestione di semafori    
    tramite operazioni di: inizializzazione, incremento e decremento.
    Inoltre viene proposta un implementazione di una Fixed Size Message Queue,
    con particolare enfasi alle operazioni di pop e push.

## Prerequisiti

L'applicazione richiede la struttura base di DisastrOS.
Una volta clonata la repo github in una directory bisognerà:

    Entrare nella cartella so_sem e in base al test che si vuole effettuare cambiare
    il makefile attualmente in uso.

## Esecuzione
$ make (per il relativo makefile)

$ ./disastrOS_test per il test dei semafori 

$ ./FSMQ_test      per il test delle FSMQ

## Come Funziona
### Semaforo
Un semaforo è una struttura costituita da una variabile intera che può
essere manipolata soltanto attraverso le operazioni di wait e post; esse
rispettivamente decrementano e incrementano il contatore all'interno del
semaforo. Ovviamente, l'implementazione del semaforo è tale per cui
solo una operazione per volta può essere effettuata.
E' bene notare che un semaforo binario (solo 0 e 1 ammessi dal
contatore) costituisce effettivamente un mutex e può essere utilizzato
come tale se il sistema non fornisce quest'ultimi. Comunque, in generale,
un semaforo può spaziare in un definito range di valori. Essi sono quindi
utilizzati per regolare l'accesso a delle risorse (wait per richedere
una risorsa e signal quando rilasciata/creata) o per sincronizzare processi.

### Fixed Size Message Queue
Una coda di messaggi a dimensione fissa è una struttura dati che, tramite l'utilizzo
di semafori generici, garantisce lo scambio la sincronizzazione e mutua esclusione per
lo scambio di messaggi tra processi.
la sincronizzazione viene garantita nella fase di pushBack e popFront.
