#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[]){

    
    paquet_t paquet;
    paquet_t ack;
    int fin = 0;
    int num_seq = 0;


    init_reseau(RECEPTION);

    while (!fin)
    {
        de_reseau(&paquet);

        if(paquet.type == DATA && verifier_somme_ctrl(&paquet)){
            if(paquet.num_seq ==  num_seq){
                ack.type = ACK;
                ack.num_seq = paquet.num_seq;
                ack.lg_info = 0;
                ack.somme_ctrl =  generer_somme_ctrl(&ack);

                vers_reseau(&ack);
                fin =vers_application(paquet.info, paquet.lg_info);
                num_seq = (num_seq + 1) % 2;
            }else{
                ack.type = ACK;
                ack.num_seq = paquet.num_seq;
                ack.lg_info = 0;
                ack.somme_ctrl =  generer_somme_ctrl(&ack);

                vers_reseau(&ack);

            }  
        }else if(!(paquet.type == DATA && verifier_somme_ctrl(&paquet))){
            ack.type = NACK;
            ack.num_seq = paquet.num_seq;
            ack.lg_info = 0;
            ack.somme_ctrl =  generer_somme_ctrl(&ack);

            vers_reseau(&ack);
        }
    }
    

    return 0;
}