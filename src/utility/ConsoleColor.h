#ifndef HITANDBLOW_CONSOLECOLOR_H
#define HITANDBLOW_CONSOLECOLOR_H

/* FOREGROUND */
//These codes set the actual text to the specified color
#define RESETTEXT  "\x1B[0m" //Set all colors back to normal.
#define BOLDTEXT  "\x1B[1m" //Bold.
#define FOREBLK  "\x1B[30m" //Black
#define FORERED  "\x1B[31m" //Red
#define FOREGRN  "\x1B[32m" //Green
#define FOREYEL  "\x1B[33m" //Yellow
#define FOREBLU  "\x1B[34m" //Blue
#define FOREMAG  "\x1B[35m" //Magenta
#define FORECYN  "\x1B[36m" //Cyan
#define FOREWHT  "\x1B[37m" //White

#define BOLDBLACK   "\x1B[1m\x1B[30m"      //Bold Black
#define BOLDRED     "\x1B[1m\x1B[31m"      //Bold Red
#define BOLDGREEN   "\x1B[1m\x1B[32m"      //Bold Green
#define BOLDYELLOW  "\x1B[1m\x1B[33m"      //Bold Yellow
#define BOLDBLUE    "\x1B[1m\x1B[34m"      //Bold Blue
#define BOLDMAGENTA "\x1B[1m\x1B[35m"      //Bold Magenta
#define BOLDCYAN    "\x1B[1m\x1B[36m"      //Bold Cyan
#define BOLDWHITE   "\x1B[1m\x1B[37m"      //Bold White

/* BACKGROUND */
//These codes set the background color behind the text.
#define BACKBLK "\x1B[40m"
#define BACKRED "\x1B[41m"
#define BACKGRN "\x1B[42m"
#define BACKYEL "\x1B[43m"
#define BACKBLU "\x1B[44m"
#define BACKMAG "\x1B[45m"
#define BACKCYN "\x1B[46m"
#define BACKWHT "\x1B[47m"

#define BLINK "\033[5m"
#define FASTBLINK "\033[6m"

//These will set the text color and then set it back to normal afterwards.
#define BLK(x) FOREBLK x RESETTEXT
#define RED(x) FORERED x RESETTEXT
#define GRN(x) FOREGRN x RESETTEXT
#define YEL(x) FOREYEL x RESETTEXT
#define BLU(x) FOREBLU x RESETTEXT
#define MAG(x) FOREMAG x RESETTEXT
#define CYN(x) FORECYN x RESETTEXT
#define WHT(x) FOREWHT x RESETTEXT
//Example usage: cout << BLU("This text's color is now blue!") << endl;

//These will set the text's background color then reset it back.
#define BackBLK(x) BACKBLK x RESETTEXT
#define BackRED(x) BACKRED x RESETTEXT
#define BackGRN(x) BACKGRN x RESETTEXT
#define BackYEL(x) BACKYEL x RESETTEXT
#define BackBLU(x) BACKBLU x RESETTEXT
#define BackMAG(x) BACKMAG x RESETTEXT
#define BackCYN(x) BACKCYN x RESETTEXT
#define BackWHT(x) BACKWHT x RESETTEXT
//Example usage: cout << BACKRED(FOREBLU("I am blue text on a red background!")) << endl;

#endif //HITANDBLOW_CONSOLECOLOR_H
