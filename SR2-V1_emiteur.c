#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"


int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO];
    int taille_msg;
    int num_seq = 0;
    int evenement;
    int fin;
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
            paquet.info[i] =  message[i];
        }
        paquet.somme_ctrl = generer_somme_ctrl(&paquet);

        vers_reseau(&paquet);

        fin = 0;
        while (!fin)
        {
            evenement = attendre();
            if(evenement == PAQUET_RECU){
                de_reseau(&ack);

                if(ack.type == ACK && verifier_somme_ctrl(&ack) && ack.num_seq == num_seq){
                    fin = 1;
                    de_application(message, &taille_msg);
                    num_seq = (num_seq + 1) % 2;
                }
                else if(ack.type == NACK && verifier_somme_ctrl(&ack) && ack.num_seq == num_seq){
                    vers_reseau(&paquet);
                }
            }
        }
        
    }
    

    return 0;
}