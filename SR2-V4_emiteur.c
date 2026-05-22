#include<stdio.h>
#include<stdlib.h>
#include"couche_transport.h"
#include"application.h"
#include"services_reseau.h"

#define MAX_SEQ_SIZE 16
#define TEMP 100

int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO];
    int taille_msg;

    int base = 0;
    int nextseq = 0;
    int taille_fenetre = 4;

    int ack_recu[MAX_SEQ_SIZE];
    for(int i = 0; i < MAX_SEQ_SIZE; ++i){
        ack_recu[i] = 0;
    }

    paquet_t paquet;
    paquet_t ack;
    paquet_t buffer[MAX_SEQ_SIZE];
    int evenement;
    int fenetre;
    if(argc == 2){
        fenetre = atoi(argv[1]);
        if(fenetre > 0 && fenetre <= MAX_SEQ_SIZE / 2){
            taille_fenetre = fenetre;
        }else{
            printf("Erreur");
            return 1;
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

            ack_recu[nextseq] = 0;
            buffer[nextseq] = paquet;
            vers_reseau(&paquet);
            depart_temporisateur_num(nextseq, TEMP);

            nextseq = (nextseq + 1) % MAX_SEQ_SIZE;
            de_application(message, &taille_msg);
        }

        evenement = attendre();

        if(evenement == PAQUET_RECU){
            de_reseau(&ack);
            if(ack.type == ACK && verifier_somme_ctrl(&ack)){
                if(dans_fenetre(base, ack.num_seq, taille_fenetre)){
                    arret_temporisateur_num(ack.num_seq);
                    ack_recu[ack.num_seq] = 1;
                    while (ack_recu[base] == 1)
                    {
                        ack_recu[base] = 0;
                        base = (base + 1) % MAX_SEQ_SIZE;
                    }
                }
            }
        }else if(evenement >= 0){
            if(dans_fenetre(base, evenement, taille_fenetre)){
                vers_reseau(&buffer[evenement]);
                depart_temporisateur_num(evenement, TEMP);
            }
        }
    }
    return 0;
}