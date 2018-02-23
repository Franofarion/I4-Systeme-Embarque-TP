/*
 * File:   test_IO.c
 * Author: didier Coque *
 * Created on 17 février 2017, 11:30
 */
#include <xc.h>
#include "pic16f877.h"
// fichier config des bits spécifiques au processeur 16f877
// à utiliser dans les autres projets utilisant ce processeur
#include "config_bits.h"

void main(void)
{
    int selectedTemperature = 30;
    int currentTemperature = 31;
    int goUp = 0;
    int goDown = 0;
    
    
    int toggleButtonRC0 = 0;
    int toggleButtonRC1 = 0;
    // configuration des ports 
    // voir doc 16f877 page 33 
    TRISC = 0b00000011;  //  Port C en  sortie 
    TRISD = 0b11111100;  //port D  tout en entree  
    PORTC = 0b00100011;
   // ajouter les lignes RD0 et RD1 dans les i/o pins du simulateur 
   // ajouter PORTC en watch run time ( Debug -> new runtime watch))
    // tester en simulation 
    while (1)
    {
       
        
        if((PORTC & 0b00000001) == 0b00000001 && toggleButtonRC0 == 0){//temp - est appuyé
            toggleButtonRC0 = 1;
            if(selectedTemperature - 4 >= 22){
                selectedTemperature -= 4;
            }
        }
        
        if((PORTC & 0b00000001) == 0b0000000){
            toggleButtonRC0 = 0;
        }
        
        
        if((PORTC & 0b00000010) == 0b00000010 && toggleButtonRC1 == 0){//temp +  est appuyé
            toggleButtonRC1 = 1;
            if(selectedTemperature + 4 <= 38){
                selectedTemperature += 4;
            }
        }
        
        if((PORTC & 0b00000010) == 0b00000000){
            toggleButtonRC1 = 0;   
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
                PORTD = 0b11111110;
            }else{
                if(currentTemperature >= selectedTemperature){
                    goUp = 0;
                } 
                if(!goUp){
                    //eteins
                    PORTD = 0b11111100;
                }else{
                    //allume
                    PORTD = 0b11111110;
                }
            }
        }else{
        if(currentTemperature >= selectedTemperature + 1){
            goDown = 1;
            PORTD = 0b11111101;
            }else{
                if(currentTemperature >= selectedTemperature){
                    goDown = 0;
                }

                if(!goDown){
                    PORTD = 0b11111100;
                }else{
                    PORTD = 0b11111101;
                }
            }   
        }
        
       
     }             
}
