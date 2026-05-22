#include<stdio.h>
#include<stdlib.h>
#include"application.h"
#include"services_reseau.h"
#include"couche_transport.h"

#define MAX_SEQ_SIZE 16
#define TEMP 100

int main(int argc, char* argv[]){

    int fin = 0;

    int base = 0;
    int taille_fenetre = 4;

    int evenement;
    int dernier_paqu = 0; 
    int ack_recu[MAX_SEQ_SIZE];
    for(int i = 0; i < MAX_SEQ_SIZE; ++i){
        ack_recu[i] = 0;
    }
    
    paquet_t paquet;
    paquet_t ack;
    paquet_t buffer[MAX_SEQ_SIZE];
    
    int fenetre;
    if(argc == 2){
        fenetre = atoi(argv[1]);
        if(fenetre > 0 && fenetre <= MAX_SEQ_SIZE /2){
            taille_fenetre = fenetre;
        }else{
            printf("Erreur");
            return 1;
        }
    }else if(argc >2){
        printf("Erreur");
        return 1;
    }

    init_reseau(RECEPTION);

    while (!fin)
    {
        de_reseau(&paquet);
        if(paquet.type == DATA && verifier_somme_ctrl(&paquet)){
            if(dans_fenetre(base, paquet.num_seq, taille_fenetre)){
                
                ack.type = ACK;
                ack.num_seq = paquet.num_seq;
                ack.lg_info = 0;
                ack.somme_ctrl = generer_somme_ctrl(&ack);

                vers_reseau(&ack);

                dernier_paqu =paquet.num_seq;
                if (ack_recu[paquet.num_seq] == 0){
                    ack_recu[paquet.num_seq] = 1;
                    buffer[paquet.num_seq] = paquet;
                }
                while (ack_recu[base] == 1)
                {
                    fin = vers_application(buffer[base].info, buffer[base].lg_info);
                    ack_recu[base] = 0;
                    base = (base + 1) % MAX_SEQ_SIZE;
                }
            }else{
                ack.type = ACK;
                ack.num_seq = paquet.num_seq;
                ack.lg_info = 0;
                ack.somme_ctrl = generer_somme_ctrl(&ack);

                vers_reseau(&ack);
            }
        }
    }
    depart_temporisateur(TEMP);
    int attenddu_fin  = 1;
    while (attenddu_fin)
    {
        evenement = attendre();
        if(evenement == PAQUET_RECU){
            de_reseau(&paquet);

            ack.type = ACK;
            ack.num_seq = dernier_paqu;
            ack.lg_info = 0;
            ack.somme_ctrl = generer_somme_ctrl(&ack);

            vers_reseau(&ack);
            depart_temporisateur(TEMP);
            dernier_paqu = paquet.num_seq;
        }else if(evenement >= 0){
            attenddu_fin = 0;
        }
    }
    
    return 0;
}