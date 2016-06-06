/*
 * Soubor:    proj01.c
 * Autor:     Radim KUBIŠ, xkubis03
 * Vytvořeno: 2. dubna 2014
 *
 * Projekt č. 1 do předmětu Pokročilé operační systémy (POS).
 */

#define _XOPEN_SOURCE

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

// Proměnná se znakem pro výpis
char letter = 'A';
// Pomocná proměnná pro detekci příchozího signálu SIGUSR1
int signaled = 0;
// Proměnná pro uložení PIDu z funkce fork())
pid_t forkPid;
// Proměnná pro návratový kód
int returnValue = EXIT_SUCCESS;

/*
 * Funkce volaná při přijetí signálu SIGUSR2,
 * zresetuje znak pro výpis na znak 'A'.
 *
 * sig - číslo příchozího signálu
 */
void resetLetter(int sig) {
    // Nastavení znaku na 'A'
    letter = 'A';
}

/*
 * Funkce volaná při přijetí signálu SIGUSR1,
 * nastaví pomocnou proměnnou signaled na 1.
 *
 * sig - číslo příchozího signálu
 */
void sigUsr1(int sig) {
    // Nastavení proměnné na 1
    signaled = 1;
}

/*
 * Funkce inkrementující/cyklící vypisovaný znak.
 */
void incrementLetter() {
    // Pokud ještě nebylo dosaženo znaku 'Z'
    if(letter < 'Z') {
        // Inkrementace na další znak
        letter++;
    } else {
        // Jinak zresetování na znak 'A'
        letter = 'A';
    }
}

/*
 * Funkce volaná při přijetí signálu SIGINT,
 * pouze pro rodičovský proces, který čeká
 * na ukončení synovského procesu.
 *
 * sig - číslo příchozího signálu
 */
void sigInt(int sig) {
    // Proměnná pro status synovského procesu
    int status;
    // Čekání na ukončení synovského procesu
    int result = waitpid(forkPid, &status, 0);
    // Kontrola výsledku čekání
    if (result == -1) {
        // Tisk chyby čekání
        perror("waitpid");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }

    // Kontrola způsobu ukončení synovského procesu
    if(WIFEXITED(status)) {
        // Logický AND návratových hodnot procesů
        returnValue = returnValue | WEXITSTATUS(status);
    }

    // Ukončení rodičovského procesu
    exit(returnValue);
}

/*
 * Hlavní funkce main.
 *
 * argc - počet argumentů příkazového řádku
 * argv - argumenty příkazové řádky
 *
 * Návratová hodnota:
 *         0 - program proběhl v pořádku
 *     jinak - program skončil s chybou
 */
int main(int argc, char *argv[]) {
    // Proměnná pro návratové hodnoty volaných funkcí
    int result;
    // PID vlastního procesu
    pid_t myPid;
    // Struktura pro signál SIGUSR1
    struct sigaction sigUser1Action;
    // Struktura pro signál SIGUSR2
    struct sigaction sigUser2Action;
    // Struktura pro signál SIGINT
    struct sigaction sigIntAction;
    // Struktura pro uložení starého nastavení signálů
    struct sigaction sigIntActionOld;
    // Seznam pro signál SIGUSR1
    sigset_t sigUser1Set;
    // Seznam pro signál SIGUSR2
    sigset_t sigUser2Set;
    // Prázdný seznam signálů
    sigset_t emptySet;

    // Nastavení struktury pro příchozí signál SIGUSR1
    // Nastavení funkce zpracovávající signál
    sigUser1Action.sa_handler = sigUsr1;
    // Vyprázdnění seznamu signálů
    result = sigemptyset(&sigUser1Action.sa_mask);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser1Action sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení příznaků
    sigUser1Action.sa_flags = 0;

    // Nastavení struktury pro příchozí signál SIGUSR2
    // Nastavení funkce zpracovávající signál
    sigUser2Action.sa_handler = resetLetter;
    // Vyprázdnění seznamu signálů
    result = sigemptyset(&sigUser2Action.sa_mask);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser2Action sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení příznaků
    sigUser2Action.sa_flags = 0;

    // Nastavení struktury pro příchozí signál SIGINT
    // Nastavení funkce zpracovávající signál
    sigIntAction.sa_handler = sigInt;
    // Vyprázdnění seznamu signálů
    result = sigemptyset(&sigIntAction.sa_mask);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigIntAction sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení příznaků
    sigIntAction.sa_flags = 0;

    // Nastavení seznamu signálů sigUser1Set na prázdný seznam
    result = sigemptyset(&sigUser1Set);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser1Set sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení seznamu signálů sigUser2Set na prázdný seznam
    result = sigemptyset(&sigUser2Set);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser2Set sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení seznamu signálů emptySet na prázdný seznam
    result = sigemptyset(&emptySet);
    // Kontrola výsledku funkce sigemptyset
    if(result == -1) {
        // Tisk místa chyby
        perror("emptySet sigemptyset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }

    // Přidání signálu SIGUSR1 do seznamu pro něj
    result = sigaddset(&sigUser1Set, SIGUSR1);
    // Kontrola výsledku funkce sigaddset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser1Set sigaddset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Přidání signálu SIGUSR2 do seznamu pro něj
    result = sigaddset(&sigUser2Set, SIGUSR2);
    // Kontrola výsledku funkce sigaddset
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser2Set sigaddset");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }

    // Nastavení akce pro signál SIGUSR1
    result = sigaction(SIGUSR1, &sigUser1Action, 0);
    // Kontrola výsledku funkce sigaction
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser1Action sigaction");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení akce pro signál SIGUSR2
    result = sigaction(SIGUSR2, &sigUser2Action, 0);
    // Kontrola výsledku funkce sigaction
    if(result == -1) {
        // Tisk místa chyby
        perror("sigUser2Action sigaction");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }
    // Nastavení akce pro signál SIGINT
    result = sigaction(SIGINT, &sigIntAction, &sigIntActionOld);
    // Kontrola výsledku funkce sigaction
    if(result == -1) {
        // Tisk místa chyby
        perror("sigIntAction sigaction");
        // Ukončení programu s chybou
        exit(EXIT_FAILURE);
    }

    // Vytvoření synovského procesu
    forkPid = fork();

    if(forkPid == 0) {
        // Synovský proces

        // Uložení PIDu synovského procesu
        myPid = getpid();

        // Obnova původního chování při příchodu signálu SIGINT
        result = sigaction(SIGINT, &sigIntActionOld, 0);
        // Kontrola výsledku funkce sigaction
        if(result == -1) {
            // Tisk místa chyby
            perror("child: sigIntActionOld sigaction");
            // Volání ukončení rodičovského procesu
            kill(getppid(), SIGINT);
            // Ukončení synovského procesu s chybou
            exit(EXIT_FAILURE);
        }


        // Cyklus synovského procesu
        while(1) {
            // Zablokování signálu SIGUSR1
            result = sigprocmask(SIG_BLOCK, &sigUser1Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Tisk místa chyby
                perror("child: SIG_BLOCK sigUser1Set sigprocmask");
                // Volání ukončení rodičovského procesu
                kill(getppid(), SIGINT);
                // Ukončení synovského procesu s chybou
                exit(EXIT_FAILURE);
            }
            // Pokud nebyl přijat signál SIGUSR1
            while (!signaled) {
                // Čekám na signál
                sigsuspend(&emptySet);
            }
            // Výpis identifikace synovského procesu
            // a aktuálního znaku synovského procesu
            fprintf(stdout, "Child %6d: %c\n", (int)myPid, letter);
            // Přechod znaku na následující
            incrementLetter();
            // Odeslání signálu SIGUSR1 rodičovskému procesu
            result = kill(getppid(), SIGUSR1);
            // Kontrola výsledku funkce kill
            if(result == -1) {
                // Tisk místa chyby
                perror("child->parent kill");
                // Volání ukončení rodičovského procesu
                kill(getppid(), SIGINT);
                // Ukončení synovského procesu s chybou
                exit(EXIT_FAILURE);
            }
            // Vynulování detekce signálu SIGUSR1
            signaled = 0;
            // Odblokování signálu SIGUSR1
            result = sigprocmask(SIG_UNBLOCK, &sigUser1Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Tisk místa chyby
                perror("child: SIG_UNBLOCK sigUser1Set sigprocmask");
                // Volání ukončení rodičovského procesu
                kill(getppid(), SIGINT);
                // Ukončení synovského procesu s chybou
                exit(EXIT_FAILURE);
            }
        }
    } else if(forkPid > 0) {
        // Rodičovský proces

        // Uložení PIDu rodičovského procesu
        myPid = getpid();

        // Cyklus rodičovského procesu
        while(1) {
            // Výpis identifikace rodičovského procesu
            // a aktuálního znaku rodičovského procesu
            fprintf(stdout, "Parent %5d: %c\n", (int)myPid, letter);
            // Přechod znaku na následující
            incrementLetter();
            // Zablokování signálu SIGUSR1
            result = sigprocmask(SIG_BLOCK, &sigUser1Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Nastavení návratové hodnoty
                returnValue = EXIT_FAILURE;
                // Tisk místa chyby
                perror("parent: SIG_BLOCK sigUser1Set sigprocmask");
                // Volání ukončení synovského procesu
                kill(forkPid, SIGINT);
                // Volání ukončení rodičovského procesu
                kill(myPid, SIGINT);
            }
            // Vynulování detekce signálu SIGUSR1
            signaled = 0;
            // Odeslání signálu SIGUSR1 synovskému procesu
            result = kill(forkPid, SIGUSR1);
            // Kontrola výsledku funkce kill
            if(result == -1) {
                // Nastavení návratové hodnoty
                returnValue = EXIT_FAILURE;
                // Tisk místa chyby
                perror("parent->child kill");
                // Volání ukončení synovského procesu
                kill(forkPid, SIGINT);
                // Volání ukončení rodičovského procesu
                kill(myPid, SIGINT);
            }
            // Pokud nebyl přijat signál SIGUSR1
            while (!signaled) {
                // Čekám na signál
                sigsuspend(&emptySet);
            }
            // Odblokování signálu SIGUSR1
            result = sigprocmask(SIG_UNBLOCK, &sigUser1Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Nastavení návratové hodnoty
                returnValue = EXIT_FAILURE;
                // Tisk místa chyby
                perror("parent: SIG_UNBLOCK sigUser1Set sigprocmask");
                // Volání ukončení synovského procesu
                kill(forkPid, SIGINT);
                // Volání ukončení rodičovského procesu
                kill(myPid, SIGINT);
            }
            // Výpis výzvy na stisk klávesy Enter
            fprintf(stdout, "Press enter ...");
            // Zablokování signálu SIGUSR2
            result = sigprocmask(SIG_BLOCK, &sigUser2Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Nastavení návratové hodnoty
                returnValue = EXIT_FAILURE;
                // Tisk místa chyby
                perror("parent: SIG_BLOCK sigUser2Set sigprocmask");
                // Volání ukončení synovského procesu
                kill(forkPid, SIGINT);
                // Volání ukončení rodičovského procesu
                kill(myPid, SIGINT);
            }
            // Čekání na stisk klávesy Enter
            while(getchar() != '\n');
            // Odblokování signálu SIGUSR2
            result = sigprocmask(SIG_UNBLOCK, &sigUser2Set, NULL);
            // Kontrola výsledku funkce sigprocmask
            if(result == -1) {
                // Nastavení návratové hodnoty
                returnValue = EXIT_FAILURE;
                // Tisk místa chyby
                perror("parent: SIG_UNBLOCK sigUser2Set sigprocmask");
                // Volání ukončení synovského procesu
                kill(forkPid, SIGINT);
                // Volání ukončení rodičovského procesu
                kill(myPid, SIGINT);
            }
        }
    } else {
        // Tisk chyby při vytváření synovského procesu
        perror("fork");
        // Nepodařilo se vytvořit synovský proces
        return EXIT_FAILURE;
    }

    // Ukončení programu bez chyby
    return EXIT_SUCCESS;
}
