#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

#define TEMP 100

int main(int argc, char*argv[]){

    unsigned char message[MAX_INFO];
    int taille_msg;
    int num_seq = 0;
    int fin;
    int evenement;
    paquet_t paquet;
    paquet_t ack;

    init_reseau(EMISSION);
    de_application(message, &taille_msg);

    while (taille_msg != 0)
    {
        paquet.type = DATA;
        paquet.num_seq = num_seq;
        paquet.lg_info = taille_msg;
        for(int i = 0; i < taille_msg; ++i){
            paquet.info[i] = message[i];
        }
        paquet.somme_ctrl = generer_somme_ctrl(&paquet);

        vers_reseau(&paquet);
        depart_temporisateur(TEMP);

        fin =0;
        while (!fin)
        {
            evenement = attendre();

            if(evenement == PAQUET_RECU){
                de_reseau(&ack);
                if(ack.type == ACK && verifier_somme_ctrl(&ack) && ack.num_seq == num_seq){
                    arret_temporisateur();
                    de_application(message, &taille_msg);
                    num_seq = (num_seq + 1) % 2;
                    fin = 1;
                }
            }else if(evenement >= 0){
                vers_reseau(&paquet);
                depart_temporisateur(TEMP);
            }
        }
        
    }
    
    return 0;
}