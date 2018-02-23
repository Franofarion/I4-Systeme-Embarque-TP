/*
 * File:   test_IO.c
 * Author: didier Coque *
 * Created on 17 f�vrier 2017, 11:30
 */
#include <xc.h>
#include "pic16f877.h"
// fichier config des bits sp�cifiques au processeur 16f877
// � utiliser dans les autres projets utilisant ce processeur
#include "config_bits.h"
#define _XTAL_FREQ 4000000 

// R�cup�ration tension capteur de temp�rature
#define VREF_plus  5.0f   // vref + = Vcc tension d'alimentation
#define VREF_moins 0.0f   // vref - =  0V
#define PLEINE_ECH 1024   // convertisseur 10 bits.  pleine echelle = 2^10 = 1024
#define DEGREE_STEP 125


int timer = 0;

void main(void)
{
    int selectedTemperature = 30;
    float currentTemperature = 31;
    
    //Variable permettant le toggle du chauffage/clim
    int goUp = 0;
    int goDown = 0;
    
    
    //Toggle des boutons plus/moins temp�rature
    int toggleButtonRC0 = 0;
    int toggleButtonRC1 = 0;

    
    //Initilisation des ports
    TRISC = 0b00000011;  //  Port C en  sortie 
    TRISD = 0b11111100;  //port D  tout en entree  
    PORTC = 0b00100011;

   
    //Timer
    TRISBbits.TRISB4 = 0; // RB4 en sortie
    OPTION_REGbits.T0CS=0; // clock syst?me
    OPTION_REGbits.PSA=0;  //prescaler sur timer0
    OPTION_REGbits.PS0=1 ;
    OPTION_REGbits.PS1=1 ;
    OPTION_REGbits.PS2=1 ;  // pr?diviseurpar 256
    TMR0 = 0;
    INTCONbits.T0IE=1;   //valide it timer0 //  doc 16f877 p 20
    INTCONbits.PEIE=0;  // valide interruption p?riph?rique
    INTCONbits.T0IF = 0; //reset interruption flag
    ei();
    
    
    //Capteur temperature
    unsigned long ValeurADC = 0L;
    float tension=0.0f;
    TRISA = 0xFF;  //port A  tout en entree doc p 111
    ADCON1 = 0b10001110;   // right justified vref+=Vdd vref- = Vss RA0 an autres entr?es digitales  doc p 112
    ADCON0 = 0b01000001;   // Fosc/8 RA0 stop x AD on
    
    __delay_us(100);
    while (1)
    {
       //On a trouv� 250 comme valeur qui se rapprochait le plus de 5 secondes, malgr�s des calculs nous indiquant 83. 83 it�rations ne donnait que 2secondes de timeout
        if(timer > 250){
            timer = 0;
            ADCON0bits.GO_DONE = 1;
            while(ADCON0bits.GO_DONE==1);
            ValeurADC = ADRESH <<8; 
            ValeurADC += ADRESL;
            tension = (VREF_plus - VREF_moins)*ValeurADC / PLEINE_ECH;
            currentTemperature = tension / DEGREE_STEP * 1000;
            __delay_ms(500);
        }
        
        
        //Gestion des boutons + / - de la temp�rature
        //Grace a 2 variables TogggleButtonRCX on attend que le bouton soit relach� pour le repdrendre en compte une nouvelle fois
        if((PORTC & 0b00000001) == 0b00000000 && toggleButtonRC0 == 0){//temp - est appuy� 
            toggleButtonRC0 = 1;
            if(selectedTemperature - 4 >= 22){//On tronque la temp�rature
                selectedTemperature -= 4;
            }
        }
        
        //quand on lache le bouton RC0 on pourra r�appuy� dessus
        if((PORTC & 0b00000001) == 0b0000001){
            toggleButtonRC0 = 0;
        }
        
        
        if((PORTC & 0b00000010) == 0b00000000 && toggleButtonRC1 == 0){//temp +  est appuy�
            toggleButtonRC1 = 1;
            if(selectedTemperature + 4 <= 38){
                selectedTemperature += 4;
            }
        }
        
        if((PORTC & 0b00000010) == 0b00000010){
            toggleButtonRC1 = 0;   
        }
        
        //Affichage de la temp�rature actuelle
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
        
        
        //Code permettant la gestion de la ventilation et du chauffage.
        if(currentTemperature <= selectedTemperature){
            goDown = 0;
            if(currentTemperature <= selectedTemperature - 1){//Si la temp�rature est en dessous de la temp�rature souhait� - 1 : On doit utilie le chauffage juqu'� atteindre la temp�rature soouhait�
                goUp = 1;//On enregistre qu'on monte jusqu'� la temp�rature souhait�
                //allume
                PORTD = 0b11111110;
            }else{
                if(!goUp){//Si on est toujours au dessus de la temp�rature souhait� - 1 mais qu'on a d�j� atteint la temp�rature souhait�: on doit redescendre jusqu'� temp�rature souhait� -1.
                    //eteins
                    PORTD = 0b11111100;
                }else{
                    //allume
                    PORTD = 0b11111110;//Si on �tait en dessous de Temp�rature souhait� -1 et qu'on doit mont� jusqu'� atteindre la temp�rature souhait�
                }
            }
        }else{//M�me chose mais invers�.
            goUp = 0;
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



void interrupt  timer_0(void)
{
    //check if the interrupt is caused by the pin RB0
    if(INTCONbits.T0IF == 1)  // test source d'interruption RB0
    {
        PORTBbits.RB4 = ~PORTBbits.RB4;   // change ?tat de RB4
        TMR0 = 0;
        INTCONbits.T0IF = 0;  // remise ? 0 du flag d'interruption
        timer++;
    }
}
