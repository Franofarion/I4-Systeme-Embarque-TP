/*
 * File:   test_IO.c
 * Author: didier Coque *
 * Created on 17 février 2017, 11:30
 */
#include <xc.h>
// fichier config des bits spécifiques au processeur 16f877
// à utiliser dans les autres projets utilisant ce processeur
#include "config_bits.h"

void main(void)
{
    int selectedTemperature = 30;
    int currentTemperature = 31;
    int goUp = 0;
    int goDown = 0;
    PORTC = 0b00100011;
    // configuration des ports 
    // voir doc 16f877 page 33 
    TRISC = 0b00000000;  //  Port C en  sortie 
    TRISD = 0b11111100;  //port D  tout en entree  
   //  le bit 3 du port D est mis à 1 si les bit 0 et 1 du port D sont à 1 
   // ajouter les lignes RD0 et RD1 dans les i/o pins du simulateur 
   // ajouter PORTC en watch run time ( Debug -> new runtime watch))
    // tester en simulation 
    while (1)
    {
    
        if((PORTC & 0b00000001) == 0b00000000){//temp - est appuyé
            if(selectedTemperature - 4 >= 22){
                selectedTemperature -= 4;
            }
        }else if((PORTC & 0b00000010) == 0b00000000){//temp +  est appuyé
            if(selectedTemperature + 4 <= 38){
                selectedTemperature += 4;
            }
        }
        
        switch(selectedTemperature){
            case 22:
                PORTC = 0b00001011;
                break;
            case 26:
                PORTC = 0b00010011;
                break;
            case 30:
                PORTC = 0b00100011;
                break;
            case 34:
                PORTC = 0b01000011;
                break;
            case 38:
                PORTC = 0b10000011;
                break;
        }
        
        if(currentTemperature <= selectedTemperature){
            if(currentTemperature <= selectedTemperature - 1){
                goUp = 1;
                //allume
                TRISD = 0b11111110;
            }else{
                if(currentTemperature >= selectedTemperature){
                    goUp = 0;
                } 
                if(!goUp){
                    //eteins
                    TRISD = 0b11111100;
                }else{
                    //allume
                    TRISD = 0b11111110;
                }
            }
        }else{
        if(currentTemperature >= selectedTemperature + 1){
            goDown = 1;
            TRISD = 0b11111101;
            }else{
                if(currentTemperature >= selectedTemperature){
                    goDown = 0;
                }

                if(!goDown){
                    TRISD = 0b11111100;
                }else{
                    TRISD = 0b11111101;
                }
            }   
        }
        
        
        
    
        //Data = RD0 & RD1;  // lit l'état des broches 0 et 1 du port D  Data <- Rd0 et Rd1
                            // Data = 1 si RD0 =1 et RD1 = 1  sinon Data = 0 
        //Data = Data <<3 ;  // décale Data de 3 bit vers la gauche ( multiplication par 8))
                           // le bit 1 se retrouve dans le bit 3 
     }             
}
