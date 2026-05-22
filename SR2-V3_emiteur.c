#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

#define MAX_NUM_SIZE 16
#define TEMP 100

int main(int argc, char*argv[]){

    unsigned char message[MAX_INFO];
    int taille_msg;

    int base = 0;
    int nextseq = 0;
    int taille_fenetre = 4;
    
    int evenement;

    paquet_t paquet;
    paquet_t ack;
    paquet_t buffer[MAX_NUM_SIZE];
    
    if(argc == 2){
        int input = atoi(argv[1]);
        if(input > 0 && input < MAX_NUM_SEQ){
            taille_fenetre = input;
        }
    }else if(argc > 2){
        printf("Erreur");
        return 1;
    }
    
    init_reseau(EMISSION);
    de_application(message, &taille_msg);
    
    while (taille_msg != 0 || base != nextseq)
    {
        while (taille_msg != 0 && dans_fenetre(base, nextseq, taille_fenetre))
        {
            paquet.type = DATA;
            paquet.num_seq = nextseq;
            paquet.lg_info = taille_msg;
            for(int i = 0; i < taille_msg; ++i){
                paquet.info[i] = message[i];
            }
            paquet.somme_ctrl = generer_somme_ctrl(&paquet);

            vers_reseau(&paquet);
            buffer[nextseq] = paquet;
            if(base == nextseq){
                depart_temporisateur(TEMP);
            }
            nextseq = (nextseq + 1) % MAX_NUM_SIZE;
            de_application(message, &taille_msg); 
        }

        evenement = attendre();

        if(evenement == PAQUET_RECU){
            de_reseau(&ack);
            if(ack.type == ACK && verifier_somme_ctrl(&ack)){
                if(dans_fenetre(base, ack.num_seq, taille_fenetre)){
                    base = (ack.num_seq + 1) / MAX_NUM_SIZE;
                    if(base == nextseq){
                        arret_temporisateur();
                    }else{
                        depart_temporisateur(TEMP);
                    }
                }
            }
        }else{
            int i = base;
            while (i != nextseq)
            {
                vers_reseau(&buffer[i]);
                i = (i + 1);
            }
            
        }
        
    }
    
    return 0;
}