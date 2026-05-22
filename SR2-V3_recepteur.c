#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

#define SEQ_NUM_SIZE 16
#define TEMP 100

int main(int argc, char* argv[]){

    paquet_t paquet;
    paquet_t ack;

    int evenement;
    int attendu = 0;
    int fin = 0;
    int dernier_ack = 0;
    int attendre_fin;

    if(argc > 1){
        printf("Erreur");
        return 1;
    }
    init_reseau(RECEPTION);

    while (!fin)
    {
        de_reseau(&paquet);
        if(paquet.type == DATA && verifier_somme_ctrl(&paquet)){
            if(attendu == paquet.num_seq){
                fin = vers_application(paquet.info, paquet.lg_info);

                ack.type = ACK;
                ack.num_seq = paquet.num_seq;
                ack.lg_info = 0;
                ack.somme_ctrl = generer_somme_ctrl(&ack);

                vers_reseau(&ack);
                dernier_ack = paquet.num_seq;
                attendu = (attendu + 1) % SEQ_NUM_SIZE;
            }else{
                ack.type = ACK;
                ack.num_seq = (attendu - 1 + SEQ_NUM_SIZE) % SEQ_NUM_SIZE;
                ack.lg_info = 0;
                ack.somme_ctrl = generer_somme_ctrl(&ack);

                vers_reseau(&ack);
            }
        }else{
            ack.type = ACK;
            ack.num_seq = (attendu + SEQ_NUM_SIZE - 1) % SEQ_NUM_SIZE;
            ack.lg_info = 0;
            ack.somme_ctrl = generer_somme_ctrl(&ack);

            vers_reseau(&ack);
        }
    }

    depart_temporisateur(TEMP);
    attendre_fin = 1;
    
    while (attendre_fin)
    {
        evenement = attendre();
        if(evenement == PAQUET_RECU){
            de_reseau(&paquet);

            ack.type = ACK;
            ack.num_seq = dernier_ack;
            ack.lg_info = 0;
            ack.somme_ctrl = generer_somme_ctrl(&ack);

            vers_reseau(&ack);

            depart_temporisateur(TEMP);
        }else{
            attendre_fin = 0;
        }
    }
    
    return 0;
}