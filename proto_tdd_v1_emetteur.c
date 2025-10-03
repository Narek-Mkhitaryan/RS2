/*************************************************************
* proto_tdd_v0 -  émetteur                                   *
* TRANSFERT DE DONNEES  v0                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - émetteur  */
/* =============================== */
int main(int argc, char* argv[])
{
    unsigned char buffer [MAX_INFO]; /* message de l'application */
    int taille; /* taille du message */
    paquet_t paquet ack; /* paquet utilisé par le protocole */
    unit8_t num_seq = 0;
    int res;

    /*initialisation du reseau*/
    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");


    /* tant que l'émetteur a des données à envoyer */
    while ( taille = vers_couche_transport(buffer)) > 0 {
        creer_paquet_data(&paquet, num_seq, buffer, taille);

        do{
            vers_reseau(paquet);
            res = attendre_reseau_timeout(&ack, TIMEOUT);
            if (res == -1){
                continue;
            }
            if(ack.type == ACK && ack.num_seq == num_seq){
                num_seq = (num_seq +1) % 2;
                break;
            }
        } while (1);
        de_reseau();
    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;
}
