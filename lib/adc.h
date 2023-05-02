/* 
 * File:   adc.h
 * Author: M. Osee
 */

#ifndef ADC_H
#define	ADC_H

/**
 * En-tete de la librairie de l'ADC pour ELEC-H309
 * 
 * Ces fonctions utilisent l'ADC1 du dsPIC pour convertir le signal de AN0.
 * 
 * L'ADC peut etre utilise dans 2 modes, par adcInit() :
 *   - en "manuel" : les conversions sont demarrees en appelant adcStart(). 
 *     C'est donc la boucle principale qui rythme l'echantillonnage
 *   - declenche par le timer3 : dans ce mode, une conversion est demarree 
 *     a chaque debordement du timer3. La frequence d'echantillonnage est alors
 *     definie par PR3.
 * Comme une conversion prend plusieurs cycles d'horloge, il faut utiliser 
 * adcConversionDone() pour "poller" la fin d'une conversion.
 * L'echantillon peut alors etre lu avec adcRead().
 */


#define ADC_MANUAL_SAMPLING 0
#define ADC_TIMER3_SAMPLING 2

// Start a conversion. To be only used in ADC_MANUAL_SAMPLING mode
#define adcStart()  AD1CON1bits.SAMP = 0

// Return 1 if conversion is finished, 0 otherwise
#define adcConversionDone()  AD1CON1bits.DONE

/**
 * Initialise ADC1 to convert AN0, on 10 bits
 * Parameter:
 *  - mode : set the mode of operation. It can take 2 values:
 *     - ADC_MANUAL_SAMPLING : conversions are started by adcStart()
 *     - ADC_TIMER3_SAMPLING : sampling rate is defined by TIMER3.
 *                             ! DO NOT USE adcStart() IN THIS MODE !
 */
void adcInit(int mode);


/**
 * Returns the last conversion's result.
 * If we try to read during a conversion, this function notices and returns -32768, being an impossible value for a conversion result
 */
int adcRead(void);


#endif	/* ADC_H */

