/* 
 * File:   mcp4821.h
 * Author: M. Osee
 *
 * Created on 11 janvier 2016, 14:10
 */

#ifndef MCP4821_H
#define	MCP4821_H


#define MCP4821_CS_LAT  _LATB13
#define MCP4821_CS_TRIS _TRISB13
#define MCP4821_SCK     _RP14R
#define MCP4821_SDI     _RP15R

#define MCP4821_GAIN_X1 0x3000
#define MCP4821_GAIN_X2 0x1000

void mcp4821Init();

void mcp4821Write();

#endif	/* MCP4821_H */
