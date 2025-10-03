/*************************************************************
* proto_tdd_v0 -  récepteur                                  *
* TRANSFERT DE DONNEES  v0                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* E. Lavinal - Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include <string.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{
    paquet_t paquet, ack; /* paquet utilisé par le protocole */
    uint8_t num_seq_attendu = 0;

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {
        if (de_reseau(&paquet) == -1){
            continue;
        }

        if (paquet.type == DATA && verifier_paquet(&paquet)){
            if(paquet.num_seq == num_seq_attendu) {
                de_couche_transport (paquet.info, paquet.lg_info);

                num_seq_attendu = (num_seq_attendu + 1 ) % 2;

                creer_paquet_ack(&ack, paquet.num_seq);
            }
            else{
                creer_paquet_ack(&ack, (num_seq_attendu + 1) % 2);
            }
        }else{
            ack.lg_info = 0;
            ack.type = NACK;
            ack.num_seq = num_seq_attendu;
            ack.calculer_somme_ctrl = calculer_somme_ctrl(&ack);
        }
        

    }
    de_reseau();
    
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
