///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                   //
//       ###    ##     ## ########  #######   ######   ######## ##    ## ######## ########     ###    ######## ######## ########     //
//      ## ##   ##     ##    ##    ##     ## ##    ##  ##       ###   ## ##       ##     ##   ## ##      ##    ##       ##     ##    //
//     ##   ##  ##     ##    ##    ##     ## ##        ##       ####  ## ##       ##     ##  ##   ##     ##    ##       ##     ##    //
//    ##     ## ##     ##    ##    ##     ## ##   #### ######   ## ## ## ######   ########  ##     ##    ##    ######   ##     ##    //
//    ######### ##     ##    ##    ##     ## ##    ##  ##       ##  #### ##       ##   ##   #########    ##    ##       ##     ##    //
//    ##     ## ##     ##    ##    ##     ## ##    ##  ##       ##   ### ##       ##    ##  ##     ##    ##    ##       ##     ##    //
//    ##     ##  #######     ##     #######   ######   ######## ##    ## ######## ##     ## ##     ##    ##    ######## ########     //
//                                                                                                                                   //
//    ######## #### ##       ########  //  ########   #######     ##    ##  #######  ########    ######## ########  #### ########    //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ###   ## ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ####  ## ##     ##    ##       ##       ##     ##  ##     ##       //
//    ######    ##  ##       ######    //  ##     ## ##     ##    ## ## ## ##     ##    ##       ######   ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ##  #### ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##        ##  ##       ##        //  ##     ## ##     ##    ##   ### ##     ##    ##       ##       ##     ##  ##     ##       //
//    ##       #### ######## ########  //  ########   #######     ##    ##  #######     ##       ######## ########  ####    ##       //
//                                                                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <PGE/String/Unicode.h>
#include "UnicodeInternal.h"

using namespace PGE;

void Unicode::fold(CircularArray<char16>& queue, char16 ch) {
    switch (ch) {
        case u'\u0041': {
            queue.pushBack(u'\u0061');
        } break;
        case u'\u0042': {
            queue.pushBack(u'\u0062');
        } break;
        case u'\u0043': {
            queue.pushBack(u'\u0063');
        } break;
        case u'\u0044': {
            queue.pushBack(u'\u0064');
        } break;
        case u'\u0045': {
            queue.pushBack(u'\u0065');
        } break;
        case u'\u0046': {
            queue.pushBack(u'\u0066');
        } break;
        case u'\u0047': {
            queue.pushBack(u'\u0067');
        } break;
        case u'\u0048': {
            queue.pushBack(u'\u0068');
        } break;
        case u'\u0049': {
            queue.pushBack(u'\u0069');
        } break;
        case u'\u004A': {
            queue.pushBack(u'\u006A');
        } break;
        case u'\u004B': [[fallthrough]];
        case u'\u212A': {
            queue.pushBack(u'\u006B');
        } break;
        case u'\u004C': {
            queue.pushBack(u'\u006C');
        } break;
        case u'\u004D': {
            queue.pushBack(u'\u006D');
        } break;
        case u'\u004E': {
            queue.pushBack(u'\u006E');
        } break;
        case u'\u004F': {
            queue.pushBack(u'\u006F');
        } break;
        case u'\u0050': {
            queue.pushBack(u'\u0070');
        } break;
        case u'\u0051': {
            queue.pushBack(u'\u0071');
        } break;
        case u'\u0052': {
            queue.pushBack(u'\u0072');
        } break;
        case u'\u0053': [[fallthrough]];
        case u'\u017F': {
            queue.pushBack(u'\u0073');
        } break;
        case u'\u0054': {
            queue.pushBack(u'\u0074');
        } break;
        case u'\u0055': {
            queue.pushBack(u'\u0075');
        } break;
        case u'\u0056': {
            queue.pushBack(u'\u0076');
        } break;
        case u'\u0057': {
            queue.pushBack(u'\u0077');
        } break;
        case u'\u0058': {
            queue.pushBack(u'\u0078');
        } break;
        case u'\u0059': {
            queue.pushBack(u'\u0079');
        } break;
        case u'\u005A': {
            queue.pushBack(u'\u007A');
        } break;
        case u'\u00B5': [[fallthrough]];
        case u'\u039C': {
            queue.pushBack(u'\u03BC');
        } break;
        case u'\u00C0': {
            queue.pushBack(u'\u00E0');
        } break;
        case u'\u00C1': {
            queue.pushBack(u'\u00E1');
        } break;
        case u'\u00C2': {
            queue.pushBack(u'\u00E2');
        } break;
        case u'\u00C3': {
            queue.pushBack(u'\u00E3');
        } break;
        case u'\u00C4': {
            queue.pushBack(u'\u00E4');
        } break;
        case u'\u00C5': [[fallthrough]];
        case u'\u212B': {
            queue.pushBack(u'\u00E5');
        } break;
        case u'\u00C6': {
            queue.pushBack(u'\u00E6');
        } break;
        case u'\u00C7': {
            queue.pushBack(u'\u00E7');
        } break;
        case u'\u00C8': {
            queue.pushBack(u'\u00E8');
        } break;
        case u'\u00C9': {
            queue.pushBack(u'\u00E9');
        } break;
        case u'\u00CA': {
            queue.pushBack(u'\u00EA');
        } break;
        case u'\u00CB': {
            queue.pushBack(u'\u00EB');
        } break;
        case u'\u00CC': {
            queue.pushBack(u'\u00EC');
        } break;
        case u'\u00CD': {
            queue.pushBack(u'\u00ED');
        } break;
        case u'\u00CE': {
            queue.pushBack(u'\u00EE');
        } break;
        case u'\u00CF': {
            queue.pushBack(u'\u00EF');
        } break;
        case u'\u00D0': {
            queue.pushBack(u'\u00F0');
        } break;
        case u'\u00D1': {
            queue.pushBack(u'\u00F1');
        } break;
        case u'\u00D2': {
            queue.pushBack(u'\u00F2');
        } break;
        case u'\u00D3': {
            queue.pushBack(u'\u00F3');
        } break;
        case u'\u00D4': {
            queue.pushBack(u'\u00F4');
        } break;
        case u'\u00D5': {
            queue.pushBack(u'\u00F5');
        } break;
        case u'\u00D6': {
            queue.pushBack(u'\u00F6');
        } break;
        case u'\u00D8': {
            queue.pushBack(u'\u00F8');
        } break;
        case u'\u00D9': {
            queue.pushBack(u'\u00F9');
        } break;
        case u'\u00DA': {
            queue.pushBack(u'\u00FA');
        } break;
        case u'\u00DB': {
            queue.pushBack(u'\u00FB');
        } break;
        case u'\u00DC': {
            queue.pushBack(u'\u00FC');
        } break;
        case u'\u00DD': {
            queue.pushBack(u'\u00FD');
        } break;
        case u'\u00DE': {
            queue.pushBack(u'\u00FE');
        } break;
        case u'\u00DF': [[fallthrough]];
        case u'\u1E9E': {
            queue.pushBack(u'\u0073');
            queue.pushBack(u'\u0073');
        } break;
        case u'\u0100': {
            queue.pushBack(u'\u0101');
        } break;
        case u'\u0102': {
            queue.pushBack(u'\u0103');
        } break;
        case u'\u0104': {
            queue.pushBack(u'\u0105');
        } break;
        case u'\u0106': {
            queue.pushBack(u'\u0107');
        } break;
        case u'\u0108': {
            queue.pushBack(u'\u0109');
        } break;
        case u'\u010A': {
            queue.pushBack(u'\u010B');
        } break;
        case u'\u010C': {
            queue.pushBack(u'\u010D');
        } break;
        case u'\u010E': {
            queue.pushBack(u'\u010F');
        } break;
        case u'\u0110': {
            queue.pushBack(u'\u0111');
        } break;
        case u'\u0112': {
            queue.pushBack(u'\u0113');
        } break;
        case u'\u0114': {
            queue.pushBack(u'\u0115');
        } break;
        case u'\u0116': {
            queue.pushBack(u'\u0117');
        } break;
        case u'\u0118': {
            queue.pushBack(u'\u0119');
        } break;
        case u'\u011A': {
            queue.pushBack(u'\u011B');
        } break;
        case u'\u011C': {
            queue.pushBack(u'\u011D');
        } break;
        case u'\u011E': {
            queue.pushBack(u'\u011F');
        } break;
        case u'\u0120': {
            queue.pushBack(u'\u0121');
        } break;
        case u'\u0122': {
            queue.pushBack(u'\u0123');
        } break;
        case u'\u0124': {
            queue.pushBack(u'\u0125');
        } break;
        case u'\u0126': {
            queue.pushBack(u'\u0127');
        } break;
        case u'\u0128': {
            queue.pushBack(u'\u0129');
        } break;
        case u'\u012A': {
            queue.pushBack(u'\u012B');
        } break;
        case u'\u012C': {
            queue.pushBack(u'\u012D');
        } break;
        case u'\u012E': {
            queue.pushBack(u'\u012F');
        } break;
        case u'\u0130': {
            queue.pushBack(u'\u0069');
            queue.pushBack(u'\u0307');
        } break;
        case u'\u0132': {
            queue.pushBack(u'\u0133');
        } break;
        case u'\u0134': {
            queue.pushBack(u'\u0135');
        } break;
        case u'\u0136': {
            queue.pushBack(u'\u0137');
        } break;
        case u'\u0139': {
            queue.pushBack(u'\u013A');
        } break;
        case u'\u013B': {
            queue.pushBack(u'\u013C');
        } break;
        case u'\u013D': {
            queue.pushBack(u'\u013E');
        } break;
        case u'\u013F': {
            queue.pushBack(u'\u0140');
        } break;
        case u'\u0141': {
            queue.pushBack(u'\u0142');
        } break;
        case u'\u0143': {
            queue.pushBack(u'\u0144');
        } break;
        case u'\u0145': {
            queue.pushBack(u'\u0146');
        } break;
        case u'\u0147': {
            queue.pushBack(u'\u0148');
        } break;
        case u'\u0149': {
            queue.pushBack(u'\u02BC');
            queue.pushBack(u'\u006E');
        } break;
        case u'\u014A': {
            queue.pushBack(u'\u014B');
        } break;
        case u'\u014C': {
            queue.pushBack(u'\u014D');
        } break;
        case u'\u014E': {
            queue.pushBack(u'\u014F');
        } break;
        case u'\u0150': {
            queue.pushBack(u'\u0151');
        } break;
        case u'\u0152': {
            queue.pushBack(u'\u0153');
        } break;
        case u'\u0154': {
            queue.pushBack(u'\u0155');
        } break;
        case u'\u0156': {
            queue.pushBack(u'\u0157');
        } break;
        case u'\u0158': {
            queue.pushBack(u'\u0159');
        } break;
        case u'\u015A': {
            queue.pushBack(u'\u015B');
        } break;
        case u'\u015C': {
            queue.pushBack(u'\u015D');
        } break;
        case u'\u015E': {
            queue.pushBack(u'\u015F');
        } break;
        case u'\u0160': {
            queue.pushBack(u'\u0161');
        } break;
        case u'\u0162': {
            queue.pushBack(u'\u0163');
        } break;
        case u'\u0164': {
            queue.pushBack(u'\u0165');
        } break;
        case u'\u0166': {
            queue.pushBack(u'\u0167');
        } break;
        case u'\u0168': {
            queue.pushBack(u'\u0169');
        } break;
        case u'\u016A': {
            queue.pushBack(u'\u016B');
        } break;
        case u'\u016C': {
            queue.pushBack(u'\u016D');
        } break;
        case u'\u016E': {
            queue.pushBack(u'\u016F');
        } break;
        case u'\u0170': {
            queue.pushBack(u'\u0171');
        } break;
        case u'\u0172': {
            queue.pushBack(u'\u0173');
        } break;
        case u'\u0174': {
            queue.pushBack(u'\u0175');
        } break;
        case u'\u0176': {
            queue.pushBack(u'\u0177');
        } break;
        case u'\u0178': {
            queue.pushBack(u'\u00FF');
        } break;
        case u'\u0179': {
            queue.pushBack(u'\u017A');
        } break;
        case u'\u017B': {
            queue.pushBack(u'\u017C');
        } break;
        case u'\u017D': {
            queue.pushBack(u'\u017E');
        } break;
        case u'\u0181': {
            queue.pushBack(u'\u0253');
        } break;
        case u'\u0182': {
            queue.pushBack(u'\u0183');
        } break;
        case u'\u0184': {
            queue.pushBack(u'\u0185');
        } break;
        case u'\u0186': {
            queue.pushBack(u'\u0254');
        } break;
        case u'\u0187': {
            queue.pushBack(u'\u0188');
        } break;
        case u'\u0189': {
            queue.pushBack(u'\u0256');
        } break;
        case u'\u018A': {
            queue.pushBack(u'\u0257');
        } break;
        case u'\u018B': {
            queue.pushBack(u'\u018C');
        } break;
        case u'\u018E': {
            queue.pushBack(u'\u01DD');
        } break;
        case u'\u018F': {
            queue.pushBack(u'\u0259');
        } break;
        case u'\u0190': {
            queue.pushBack(u'\u025B');
        } break;
        case u'\u0191': {
            queue.pushBack(u'\u0192');
        } break;
        case u'\u0193': {
            queue.pushBack(u'\u0260');
        } break;
        case u'\u0194': {
            queue.pushBack(u'\u0263');
        } break;
        case u'\u0196': {
            queue.pushBack(u'\u0269');
        } break;
        case u'\u0197': {
            queue.pushBack(u'\u0268');
        } break;
        case u'\u0198': {
            queue.pushBack(u'\u0199');
        } break;
        case u'\u019C': {
            queue.pushBack(u'\u026F');
        } break;
        case u'\u019D': {
            queue.pushBack(u'\u0272');
        } break;
        case u'\u019F': {
            queue.pushBack(u'\u0275');
        } break;
        case u'\u01A0': {
            queue.pushBack(u'\u01A1');
        } break;
        case u'\u01A2': {
            queue.pushBack(u'\u01A3');
        } break;
        case u'\u01A4': {
            queue.pushBack(u'\u01A5');
        } break;
        case u'\u01A6': {
            queue.pushBack(u'\u0280');
        } break;
        case u'\u01A7': {
            queue.pushBack(u'\u01A8');
        } break;
        case u'\u01A9': {
            queue.pushBack(u'\u0283');
        } break;
        case u'\u01AC': {
            queue.pushBack(u'\u01AD');
        } break;
        case u'\u01AE': {
            queue.pushBack(u'\u0288');
        } break;
        case u'\u01AF': {
            queue.pushBack(u'\u01B0');
        } break;
        case u'\u01B1': {
            queue.pushBack(u'\u028A');
        } break;
        case u'\u01B2': {
            queue.pushBack(u'\u028B');
        } break;
        case u'\u01B3': {
            queue.pushBack(u'\u01B4');
        } break;
        case u'\u01B5': {
            queue.pushBack(u'\u01B6');
        } break;
        case u'\u01B7': {
            queue.pushBack(u'\u0292');
        } break;
        case u'\u01B8': {
            queue.pushBack(u'\u01B9');
        } break;
        case u'\u01BC': {
            queue.pushBack(u'\u01BD');
        } break;
        case u'\u01C4': [[fallthrough]];
        case u'\u01C5': {
            queue.pushBack(u'\u01C6');
        } break;
        case u'\u01C7': [[fallthrough]];
        case u'\u01C8': {
            queue.pushBack(u'\u01C9');
        } break;
        case u'\u01CA': [[fallthrough]];
        case u'\u01CB': {
            queue.pushBack(u'\u01CC');
        } break;
        case u'\u01CD': {
            queue.pushBack(u'\u01CE');
        } break;
        case u'\u01CF': {
            queue.pushBack(u'\u01D0');
        } break;
        case u'\u01D1': {
            queue.pushBack(u'\u01D2');
        } break;
        case u'\u01D3': {
            queue.pushBack(u'\u01D4');
        } break;
        case u'\u01D5': {
            queue.pushBack(u'\u01D6');
        } break;
        case u'\u01D7': {
            queue.pushBack(u'\u01D8');
        } break;
        case u'\u01D9': {
            queue.pushBack(u'\u01DA');
        } break;
        case u'\u01DB': {
            queue.pushBack(u'\u01DC');
        } break;
        case u'\u01DE': {
            queue.pushBack(u'\u01DF');
        } break;
        case u'\u01E0': {
            queue.pushBack(u'\u01E1');
        } break;
        case u'\u01E2': {
            queue.pushBack(u'\u01E3');
        } break;
        case u'\u01E4': {
            queue.pushBack(u'\u01E5');
        } break;
        case u'\u01E6': {
            queue.pushBack(u'\u01E7');
        } break;
        case u'\u01E8': {
            queue.pushBack(u'\u01E9');
        } break;
        case u'\u01EA': {
            queue.pushBack(u'\u01EB');
        } break;
        case u'\u01EC': {
            queue.pushBack(u'\u01ED');
        } break;
        case u'\u01EE': {
            queue.pushBack(u'\u01EF');
        } break;
        case u'\u01F0': {
            queue.pushBack(u'\u006A');
            queue.pushBack(u'\u030C');
        } break;
        case u'\u01F1': [[fallthrough]];
        case u'\u01F2': {
            queue.pushBack(u'\u01F3');
        } break;
        case u'\u01F4': {
            queue.pushBack(u'\u01F5');
        } break;
        case u'\u01F6': {
            queue.pushBack(u'\u0195');
        } break;
        case u'\u01F7': {
            queue.pushBack(u'\u01BF');
        } break;
        case u'\u01F8': {
            queue.pushBack(u'\u01F9');
        } break;
        case u'\u01FA': {
            queue.pushBack(u'\u01FB');
        } break;
        case u'\u01FC': {
            queue.pushBack(u'\u01FD');
        } break;
        case u'\u01FE': {
            queue.pushBack(u'\u01FF');
        } break;
        case u'\u0200': {
            queue.pushBack(u'\u0201');
        } break;
        case u'\u0202': {
            queue.pushBack(u'\u0203');
        } break;
        case u'\u0204': {
            queue.pushBack(u'\u0205');
        } break;
        case u'\u0206': {
            queue.pushBack(u'\u0207');
        } break;
        case u'\u0208': {
            queue.pushBack(u'\u0209');
        } break;
        case u'\u020A': {
            queue.pushBack(u'\u020B');
        } break;
        case u'\u020C': {
            queue.pushBack(u'\u020D');
        } break;
        case u'\u020E': {
            queue.pushBack(u'\u020F');
        } break;
        case u'\u0210': {
            queue.pushBack(u'\u0211');
        } break;
        case u'\u0212': {
            queue.pushBack(u'\u0213');
        } break;
        case u'\u0214': {
            queue.pushBack(u'\u0215');
        } break;
        case u'\u0216': {
            queue.pushBack(u'\u0217');
        } break;
        case u'\u0218': {
            queue.pushBack(u'\u0219');
        } break;
        case u'\u021A': {
            queue.pushBack(u'\u021B');
        } break;
        case u'\u021C': {
            queue.pushBack(u'\u021D');
        } break;
        case u'\u021E': {
            queue.pushBack(u'\u021F');
        } break;
        case u'\u0220': {
            queue.pushBack(u'\u019E');
        } break;
        case u'\u0222': {
            queue.pushBack(u'\u0223');
        } break;
        case u'\u0224': {
            queue.pushBack(u'\u0225');
        } break;
        case u'\u0226': {
            queue.pushBack(u'\u0227');
        } break;
        case u'\u0228': {
            queue.pushBack(u'\u0229');
        } break;
        case u'\u022A': {
            queue.pushBack(u'\u022B');
        } break;
        case u'\u022C': {
            queue.pushBack(u'\u022D');
        } break;
        case u'\u022E': {
            queue.pushBack(u'\u022F');
        } break;
        case u'\u0230': {
            queue.pushBack(u'\u0231');
        } break;
        case u'\u0232': {
            queue.pushBack(u'\u0233');
        } break;
        case u'\u023A': {
            queue.pushBack(u'\u2C65');
        } break;
        case u'\u023B': {
            queue.pushBack(u'\u023C');
        } break;
        case u'\u023D': {
            queue.pushBack(u'\u019A');
        } break;
        case u'\u023E': {
            queue.pushBack(u'\u2C66');
        } break;
        case u'\u0241': {
            queue.pushBack(u'\u0242');
        } break;
        case u'\u0243': {
            queue.pushBack(u'\u0180');
        } break;
        case u'\u0244': {
            queue.pushBack(u'\u0289');
        } break;
        case u'\u0245': {
            queue.pushBack(u'\u028C');
        } break;
        case u'\u0246': {
            queue.pushBack(u'\u0247');
        } break;
        case u'\u0248': {
            queue.pushBack(u'\u0249');
        } break;
        case u'\u024A': {
            queue.pushBack(u'\u024B');
        } break;
        case u'\u024C': {
            queue.pushBack(u'\u024D');
        } break;
        case u'\u024E': {
            queue.pushBack(u'\u024F');
        } break;
        case u'\u0345': [[fallthrough]];
        case u'\u0399': [[fallthrough]];
        case u'\u1FBE': {
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u0370': {
            queue.pushBack(u'\u0371');
        } break;
        case u'\u0372': {
            queue.pushBack(u'\u0373');
        } break;
        case u'\u0376': {
            queue.pushBack(u'\u0377');
        } break;
        case u'\u037F': {
            queue.pushBack(u'\u03F3');
        } break;
        case u'\u0386': {
            queue.pushBack(u'\u03AC');
        } break;
        case u'\u0388': {
            queue.pushBack(u'\u03AD');
        } break;
        case u'\u0389': {
            queue.pushBack(u'\u03AE');
        } break;
        case u'\u038A': {
            queue.pushBack(u'\u03AF');
        } break;
        case u'\u038C': {
            queue.pushBack(u'\u03CC');
        } break;
        case u'\u038E': {
            queue.pushBack(u'\u03CD');
        } break;
        case u'\u038F': {
            queue.pushBack(u'\u03CE');
        } break;
        case u'\u0390': [[fallthrough]];
        case u'\u1FD3': {
            queue.pushBack(u'\u03B9');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0301');
        } break;
        case u'\u0391': {
            queue.pushBack(u'\u03B1');
        } break;
        case u'\u0392': [[fallthrough]];
        case u'\u03D0': {
            queue.pushBack(u'\u03B2');
        } break;
        case u'\u0393': {
            queue.pushBack(u'\u03B3');
        } break;
        case u'\u0394': {
            queue.pushBack(u'\u03B4');
        } break;
        case u'\u0395': [[fallthrough]];
        case u'\u03F5': {
            queue.pushBack(u'\u03B5');
        } break;
        case u'\u0396': {
            queue.pushBack(u'\u03B6');
        } break;
        case u'\u0397': {
            queue.pushBack(u'\u03B7');
        } break;
        case u'\u0398': [[fallthrough]];
        case u'\u03D1': [[fallthrough]];
        case u'\u03F4': {
            queue.pushBack(u'\u03B8');
        } break;
        case u'\u039A': [[fallthrough]];
        case u'\u03F0': {
            queue.pushBack(u'\u03BA');
        } break;
        case u'\u039B': {
            queue.pushBack(u'\u03BB');
        } break;
        case u'\u039D': {
            queue.pushBack(u'\u03BD');
        } break;
        case u'\u039E': {
            queue.pushBack(u'\u03BE');
        } break;
        case u'\u039F': {
            queue.pushBack(u'\u03BF');
        } break;
        case u'\u03A0': [[fallthrough]];
        case u'\u03D6': {
            queue.pushBack(u'\u03C0');
        } break;
        case u'\u03A1': [[fallthrough]];
        case u'\u03F1': {
            queue.pushBack(u'\u03C1');
        } break;
        case u'\u03A3': [[fallthrough]];
        case u'\u03C2': {
            queue.pushBack(u'\u03C3');
        } break;
        case u'\u03A4': {
            queue.pushBack(u'\u03C4');
        } break;
        case u'\u03A5': {
            queue.pushBack(u'\u03C5');
        } break;
        case u'\u03A6': [[fallthrough]];
        case u'\u03D5': {
            queue.pushBack(u'\u03C6');
        } break;
        case u'\u03A7': {
            queue.pushBack(u'\u03C7');
        } break;
        case u'\u03A8': {
            queue.pushBack(u'\u03C8');
        } break;
        case u'\u03AA': {
            queue.pushBack(u'\u03CA');
        } break;
        case u'\u03AB': {
            queue.pushBack(u'\u03CB');
        } break;
        case u'\u03B0': [[fallthrough]];
        case u'\u1FE3': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0301');
        } break;
        case u'\u03CF': {
            queue.pushBack(u'\u03D7');
        } break;
        case u'\u03D8': {
            queue.pushBack(u'\u03D9');
        } break;
        case u'\u03DA': {
            queue.pushBack(u'\u03DB');
        } break;
        case u'\u03DC': {
            queue.pushBack(u'\u03DD');
        } break;
        case u'\u03DE': {
            queue.pushBack(u'\u03DF');
        } break;
        case u'\u03E0': {
            queue.pushBack(u'\u03E1');
        } break;
        case u'\u03E2': {
            queue.pushBack(u'\u03E3');
        } break;
        case u'\u03E4': {
            queue.pushBack(u'\u03E5');
        } break;
        case u'\u03E6': {
            queue.pushBack(u'\u03E7');
        } break;
        case u'\u03E8': {
            queue.pushBack(u'\u03E9');
        } break;
        case u'\u03EA': {
            queue.pushBack(u'\u03EB');
        } break;
        case u'\u03EC': {
            queue.pushBack(u'\u03ED');
        } break;
        case u'\u03EE': {
            queue.pushBack(u'\u03EF');
        } break;
        case u'\u03F7': {
            queue.pushBack(u'\u03F8');
        } break;
        case u'\u03F9': {
            queue.pushBack(u'\u03F2');
        } break;
        case u'\u03FA': {
            queue.pushBack(u'\u03FB');
        } break;
        case u'\u03FD': {
            queue.pushBack(u'\u037B');
        } break;
        case u'\u03FE': {
            queue.pushBack(u'\u037C');
        } break;
        case u'\u03FF': {
            queue.pushBack(u'\u037D');
        } break;
        case u'\u0400': {
            queue.pushBack(u'\u0450');
        } break;
        case u'\u0401': {
            queue.pushBack(u'\u0451');
        } break;
        case u'\u0402': {
            queue.pushBack(u'\u0452');
        } break;
        case u'\u0403': {
            queue.pushBack(u'\u0453');
        } break;
        case u'\u0404': {
            queue.pushBack(u'\u0454');
        } break;
        case u'\u0405': {
            queue.pushBack(u'\u0455');
        } break;
        case u'\u0406': {
            queue.pushBack(u'\u0456');
        } break;
        case u'\u0407': {
            queue.pushBack(u'\u0457');
        } break;
        case u'\u0408': {
            queue.pushBack(u'\u0458');
        } break;
        case u'\u0409': {
            queue.pushBack(u'\u0459');
        } break;
        case u'\u040A': {
            queue.pushBack(u'\u045A');
        } break;
        case u'\u040B': {
            queue.pushBack(u'\u045B');
        } break;
        case u'\u040C': {
            queue.pushBack(u'\u045C');
        } break;
        case u'\u040D': {
            queue.pushBack(u'\u045D');
        } break;
        case u'\u040E': {
            queue.pushBack(u'\u045E');
        } break;
        case u'\u040F': {
            queue.pushBack(u'\u045F');
        } break;
        case u'\u0410': {
            queue.pushBack(u'\u0430');
        } break;
        case u'\u0411': {
            queue.pushBack(u'\u0431');
        } break;
        case u'\u0412': [[fallthrough]];
        case u'\u1C80': {
            queue.pushBack(u'\u0432');
        } break;
        case u'\u0413': {
            queue.pushBack(u'\u0433');
        } break;
        case u'\u0414': [[fallthrough]];
        case u'\u1C81': {
            queue.pushBack(u'\u0434');
        } break;
        case u'\u0415': {
            queue.pushBack(u'\u0435');
        } break;
        case u'\u0416': {
            queue.pushBack(u'\u0436');
        } break;
        case u'\u0417': {
            queue.pushBack(u'\u0437');
        } break;
        case u'\u0418': {
            queue.pushBack(u'\u0438');
        } break;
        case u'\u0419': {
            queue.pushBack(u'\u0439');
        } break;
        case u'\u041A': {
            queue.pushBack(u'\u043A');
        } break;
        case u'\u041B': {
            queue.pushBack(u'\u043B');
        } break;
        case u'\u041C': {
            queue.pushBack(u'\u043C');
        } break;
        case u'\u041D': {
            queue.pushBack(u'\u043D');
        } break;
        case u'\u041E': [[fallthrough]];
        case u'\u1C82': {
            queue.pushBack(u'\u043E');
        } break;
        case u'\u041F': {
            queue.pushBack(u'\u043F');
        } break;
        case u'\u0420': {
            queue.pushBack(u'\u0440');
        } break;
        case u'\u0421': [[fallthrough]];
        case u'\u1C83': {
            queue.pushBack(u'\u0441');
        } break;
        case u'\u0422': [[fallthrough]];
        case u'\u1C84': [[fallthrough]];
        case u'\u1C85': {
            queue.pushBack(u'\u0442');
        } break;
        case u'\u0423': {
            queue.pushBack(u'\u0443');
        } break;
        case u'\u0424': {
            queue.pushBack(u'\u0444');
        } break;
        case u'\u0425': {
            queue.pushBack(u'\u0445');
        } break;
        case u'\u0426': {
            queue.pushBack(u'\u0446');
        } break;
        case u'\u0427': {
            queue.pushBack(u'\u0447');
        } break;
        case u'\u0428': {
            queue.pushBack(u'\u0448');
        } break;
        case u'\u0429': {
            queue.pushBack(u'\u0449');
        } break;
        case u'\u042A': [[fallthrough]];
        case u'\u1C86': {
            queue.pushBack(u'\u044A');
        } break;
        case u'\u042B': {
            queue.pushBack(u'\u044B');
        } break;
        case u'\u042C': {
            queue.pushBack(u'\u044C');
        } break;
        case u'\u042D': {
            queue.pushBack(u'\u044D');
        } break;
        case u'\u042E': {
            queue.pushBack(u'\u044E');
        } break;
        case u'\u042F': {
            queue.pushBack(u'\u044F');
        } break;
        case u'\u0460': {
            queue.pushBack(u'\u0461');
        } break;
        case u'\u0462': [[fallthrough]];
        case u'\u1C87': {
            queue.pushBack(u'\u0463');
        } break;
        case u'\u0464': {
            queue.pushBack(u'\u0465');
        } break;
        case u'\u0466': {
            queue.pushBack(u'\u0467');
        } break;
        case u'\u0468': {
            queue.pushBack(u'\u0469');
        } break;
        case u'\u046A': {
            queue.pushBack(u'\u046B');
        } break;
        case u'\u046C': {
            queue.pushBack(u'\u046D');
        } break;
        case u'\u046E': {
            queue.pushBack(u'\u046F');
        } break;
        case u'\u0470': {
            queue.pushBack(u'\u0471');
        } break;
        case u'\u0472': {
            queue.pushBack(u'\u0473');
        } break;
        case u'\u0474': {
            queue.pushBack(u'\u0475');
        } break;
        case u'\u0476': {
            queue.pushBack(u'\u0477');
        } break;
        case u'\u0478': {
            queue.pushBack(u'\u0479');
        } break;
        case u'\u047A': {
            queue.pushBack(u'\u047B');
        } break;
        case u'\u047C': {
            queue.pushBack(u'\u047D');
        } break;
        case u'\u047E': {
            queue.pushBack(u'\u047F');
        } break;
        case u'\u0480': {
            queue.pushBack(u'\u0481');
        } break;
        case u'\u048A': {
            queue.pushBack(u'\u048B');
        } break;
        case u'\u048C': {
            queue.pushBack(u'\u048D');
        } break;
        case u'\u048E': {
            queue.pushBack(u'\u048F');
        } break;
        case u'\u0490': {
            queue.pushBack(u'\u0491');
        } break;
        case u'\u0492': {
            queue.pushBack(u'\u0493');
        } break;
        case u'\u0494': {
            queue.pushBack(u'\u0495');
        } break;
        case u'\u0496': {
            queue.pushBack(u'\u0497');
        } break;
        case u'\u0498': {
            queue.pushBack(u'\u0499');
        } break;
        case u'\u049A': {
            queue.pushBack(u'\u049B');
        } break;
        case u'\u049C': {
            queue.pushBack(u'\u049D');
        } break;
        case u'\u049E': {
            queue.pushBack(u'\u049F');
        } break;
        case u'\u04A0': {
            queue.pushBack(u'\u04A1');
        } break;
        case u'\u04A2': {
            queue.pushBack(u'\u04A3');
        } break;
        case u'\u04A4': {
            queue.pushBack(u'\u04A5');
        } break;
        case u'\u04A6': {
            queue.pushBack(u'\u04A7');
        } break;
        case u'\u04A8': {
            queue.pushBack(u'\u04A9');
        } break;
        case u'\u04AA': {
            queue.pushBack(u'\u04AB');
        } break;
        case u'\u04AC': {
            queue.pushBack(u'\u04AD');
        } break;
        case u'\u04AE': {
            queue.pushBack(u'\u04AF');
        } break;
        case u'\u04B0': {
            queue.pushBack(u'\u04B1');
        } break;
        case u'\u04B2': {
            queue.pushBack(u'\u04B3');
        } break;
        case u'\u04B4': {
            queue.pushBack(u'\u04B5');
        } break;
        case u'\u04B6': {
            queue.pushBack(u'\u04B7');
        } break;
        case u'\u04B8': {
            queue.pushBack(u'\u04B9');
        } break;
        case u'\u04BA': {
            queue.pushBack(u'\u04BB');
        } break;
        case u'\u04BC': {
            queue.pushBack(u'\u04BD');
        } break;
        case u'\u04BE': {
            queue.pushBack(u'\u04BF');
        } break;
        case u'\u04C0': {
            queue.pushBack(u'\u04CF');
        } break;
        case u'\u04C1': {
            queue.pushBack(u'\u04C2');
        } break;
        case u'\u04C3': {
            queue.pushBack(u'\u04C4');
        } break;
        case u'\u04C5': {
            queue.pushBack(u'\u04C6');
        } break;
        case u'\u04C7': {
            queue.pushBack(u'\u04C8');
        } break;
        case u'\u04C9': {
            queue.pushBack(u'\u04CA');
        } break;
        case u'\u04CB': {
            queue.pushBack(u'\u04CC');
        } break;
        case u'\u04CD': {
            queue.pushBack(u'\u04CE');
        } break;
        case u'\u04D0': {
            queue.pushBack(u'\u04D1');
        } break;
        case u'\u04D2': {
            queue.pushBack(u'\u04D3');
        } break;
        case u'\u04D4': {
            queue.pushBack(u'\u04D5');
        } break;
        case u'\u04D6': {
            queue.pushBack(u'\u04D7');
        } break;
        case u'\u04D8': {
            queue.pushBack(u'\u04D9');
        } break;
        case u'\u04DA': {
            queue.pushBack(u'\u04DB');
        } break;
        case u'\u04DC': {
            queue.pushBack(u'\u04DD');
        } break;
        case u'\u04DE': {
            queue.pushBack(u'\u04DF');
        } break;
        case u'\u04E0': {
            queue.pushBack(u'\u04E1');
        } break;
        case u'\u04E2': {
            queue.pushBack(u'\u04E3');
        } break;
        case u'\u04E4': {
            queue.pushBack(u'\u04E5');
        } break;
        case u'\u04E6': {
            queue.pushBack(u'\u04E7');
        } break;
        case u'\u04E8': {
            queue.pushBack(u'\u04E9');
        } break;
        case u'\u04EA': {
            queue.pushBack(u'\u04EB');
        } break;
        case u'\u04EC': {
            queue.pushBack(u'\u04ED');
        } break;
        case u'\u04EE': {
            queue.pushBack(u'\u04EF');
        } break;
        case u'\u04F0': {
            queue.pushBack(u'\u04F1');
        } break;
        case u'\u04F2': {
            queue.pushBack(u'\u04F3');
        } break;
        case u'\u04F4': {
            queue.pushBack(u'\u04F5');
        } break;
        case u'\u04F6': {
            queue.pushBack(u'\u04F7');
        } break;
        case u'\u04F8': {
            queue.pushBack(u'\u04F9');
        } break;
        case u'\u04FA': {
            queue.pushBack(u'\u04FB');
        } break;
        case u'\u04FC': {
            queue.pushBack(u'\u04FD');
        } break;
        case u'\u04FE': {
            queue.pushBack(u'\u04FF');
        } break;
        case u'\u0500': {
            queue.pushBack(u'\u0501');
        } break;
        case u'\u0502': {
            queue.pushBack(u'\u0503');
        } break;
        case u'\u0504': {
            queue.pushBack(u'\u0505');
        } break;
        case u'\u0506': {
            queue.pushBack(u'\u0507');
        } break;
        case u'\u0508': {
            queue.pushBack(u'\u0509');
        } break;
        case u'\u050A': {
            queue.pushBack(u'\u050B');
        } break;
        case u'\u050C': {
            queue.pushBack(u'\u050D');
        } break;
        case u'\u050E': {
            queue.pushBack(u'\u050F');
        } break;
        case u'\u0510': {
            queue.pushBack(u'\u0511');
        } break;
        case u'\u0512': {
            queue.pushBack(u'\u0513');
        } break;
        case u'\u0514': {
            queue.pushBack(u'\u0515');
        } break;
        case u'\u0516': {
            queue.pushBack(u'\u0517');
        } break;
        case u'\u0518': {
            queue.pushBack(u'\u0519');
        } break;
        case u'\u051A': {
            queue.pushBack(u'\u051B');
        } break;
        case u'\u051C': {
            queue.pushBack(u'\u051D');
        } break;
        case u'\u051E': {
            queue.pushBack(u'\u051F');
        } break;
        case u'\u0520': {
            queue.pushBack(u'\u0521');
        } break;
        case u'\u0522': {
            queue.pushBack(u'\u0523');
        } break;
        case u'\u0524': {
            queue.pushBack(u'\u0525');
        } break;
        case u'\u0526': {
            queue.pushBack(u'\u0527');
        } break;
        case u'\u0528': {
            queue.pushBack(u'\u0529');
        } break;
        case u'\u052A': {
            queue.pushBack(u'\u052B');
        } break;
        case u'\u052C': {
            queue.pushBack(u'\u052D');
        } break;
        case u'\u052E': {
            queue.pushBack(u'\u052F');
        } break;
        case u'\u0531': {
            queue.pushBack(u'\u0561');
        } break;
        case u'\u0532': {
            queue.pushBack(u'\u0562');
        } break;
        case u'\u0533': {
            queue.pushBack(u'\u0563');
        } break;
        case u'\u0534': {
            queue.pushBack(u'\u0564');
        } break;
        case u'\u0535': {
            queue.pushBack(u'\u0565');
        } break;
        case u'\u0536': {
            queue.pushBack(u'\u0566');
        } break;
        case u'\u0537': {
            queue.pushBack(u'\u0567');
        } break;
        case u'\u0538': {
            queue.pushBack(u'\u0568');
        } break;
        case u'\u0539': {
            queue.pushBack(u'\u0569');
        } break;
        case u'\u053A': {
            queue.pushBack(u'\u056A');
        } break;
        case u'\u053B': {
            queue.pushBack(u'\u056B');
        } break;
        case u'\u053C': {
            queue.pushBack(u'\u056C');
        } break;
        case u'\u053D': {
            queue.pushBack(u'\u056D');
        } break;
        case u'\u053E': {
            queue.pushBack(u'\u056E');
        } break;
        case u'\u053F': {
            queue.pushBack(u'\u056F');
        } break;
        case u'\u0540': {
            queue.pushBack(u'\u0570');
        } break;
        case u'\u0541': {
            queue.pushBack(u'\u0571');
        } break;
        case u'\u0542': {
            queue.pushBack(u'\u0572');
        } break;
        case u'\u0543': {
            queue.pushBack(u'\u0573');
        } break;
        case u'\u0544': {
            queue.pushBack(u'\u0574');
        } break;
        case u'\u0545': {
            queue.pushBack(u'\u0575');
        } break;
        case u'\u0546': {
            queue.pushBack(u'\u0576');
        } break;
        case u'\u0547': {
            queue.pushBack(u'\u0577');
        } break;
        case u'\u0548': {
            queue.pushBack(u'\u0578');
        } break;
        case u'\u0549': {
            queue.pushBack(u'\u0579');
        } break;
        case u'\u054A': {
            queue.pushBack(u'\u057A');
        } break;
        case u'\u054B': {
            queue.pushBack(u'\u057B');
        } break;
        case u'\u054C': {
            queue.pushBack(u'\u057C');
        } break;
        case u'\u054D': {
            queue.pushBack(u'\u057D');
        } break;
        case u'\u054E': {
            queue.pushBack(u'\u057E');
        } break;
        case u'\u054F': {
            queue.pushBack(u'\u057F');
        } break;
        case u'\u0550': {
            queue.pushBack(u'\u0580');
        } break;
        case u'\u0551': {
            queue.pushBack(u'\u0581');
        } break;
        case u'\u0552': {
            queue.pushBack(u'\u0582');
        } break;
        case u'\u0553': {
            queue.pushBack(u'\u0583');
        } break;
        case u'\u0554': {
            queue.pushBack(u'\u0584');
        } break;
        case u'\u0555': {
            queue.pushBack(u'\u0585');
        } break;
        case u'\u0556': {
            queue.pushBack(u'\u0586');
        } break;
        case u'\u0587': {
            queue.pushBack(u'\u0565');
            queue.pushBack(u'\u0582');
        } break;
        case u'\u10A0': {
            queue.pushBack(u'\u2D00');
        } break;
        case u'\u10A1': {
            queue.pushBack(u'\u2D01');
        } break;
        case u'\u10A2': {
            queue.pushBack(u'\u2D02');
        } break;
        case u'\u10A3': {
            queue.pushBack(u'\u2D03');
        } break;
        case u'\u10A4': {
            queue.pushBack(u'\u2D04');
        } break;
        case u'\u10A5': {
            queue.pushBack(u'\u2D05');
        } break;
        case u'\u10A6': {
            queue.pushBack(u'\u2D06');
        } break;
        case u'\u10A7': {
            queue.pushBack(u'\u2D07');
        } break;
        case u'\u10A8': {
            queue.pushBack(u'\u2D08');
        } break;
        case u'\u10A9': {
            queue.pushBack(u'\u2D09');
        } break;
        case u'\u10AA': {
            queue.pushBack(u'\u2D0A');
        } break;
        case u'\u10AB': {
            queue.pushBack(u'\u2D0B');
        } break;
        case u'\u10AC': {
            queue.pushBack(u'\u2D0C');
        } break;
        case u'\u10AD': {
            queue.pushBack(u'\u2D0D');
        } break;
        case u'\u10AE': {
            queue.pushBack(u'\u2D0E');
        } break;
        case u'\u10AF': {
            queue.pushBack(u'\u2D0F');
        } break;
        case u'\u10B0': {
            queue.pushBack(u'\u2D10');
        } break;
        case u'\u10B1': {
            queue.pushBack(u'\u2D11');
        } break;
        case u'\u10B2': {
            queue.pushBack(u'\u2D12');
        } break;
        case u'\u10B3': {
            queue.pushBack(u'\u2D13');
        } break;
        case u'\u10B4': {
            queue.pushBack(u'\u2D14');
        } break;
        case u'\u10B5': {
            queue.pushBack(u'\u2D15');
        } break;
        case u'\u10B6': {
            queue.pushBack(u'\u2D16');
        } break;
        case u'\u10B7': {
            queue.pushBack(u'\u2D17');
        } break;
        case u'\u10B8': {
            queue.pushBack(u'\u2D18');
        } break;
        case u'\u10B9': {
            queue.pushBack(u'\u2D19');
        } break;
        case u'\u10BA': {
            queue.pushBack(u'\u2D1A');
        } break;
        case u'\u10BB': {
            queue.pushBack(u'\u2D1B');
        } break;
        case u'\u10BC': {
            queue.pushBack(u'\u2D1C');
        } break;
        case u'\u10BD': {
            queue.pushBack(u'\u2D1D');
        } break;
        case u'\u10BE': {
            queue.pushBack(u'\u2D1E');
        } break;
        case u'\u10BF': {
            queue.pushBack(u'\u2D1F');
        } break;
        case u'\u10C0': {
            queue.pushBack(u'\u2D20');
        } break;
        case u'\u10C1': {
            queue.pushBack(u'\u2D21');
        } break;
        case u'\u10C2': {
            queue.pushBack(u'\u2D22');
        } break;
        case u'\u10C3': {
            queue.pushBack(u'\u2D23');
        } break;
        case u'\u10C4': {
            queue.pushBack(u'\u2D24');
        } break;
        case u'\u10C5': {
            queue.pushBack(u'\u2D25');
        } break;
        case u'\u10C7': {
            queue.pushBack(u'\u2D27');
        } break;
        case u'\u10CD': {
            queue.pushBack(u'\u2D2D');
        } break;
        case u'\u13F8': {
            queue.pushBack(u'\u13F0');
        } break;
        case u'\u13F9': {
            queue.pushBack(u'\u13F1');
        } break;
        case u'\u13FA': {
            queue.pushBack(u'\u13F2');
        } break;
        case u'\u13FB': {
            queue.pushBack(u'\u13F3');
        } break;
        case u'\u13FC': {
            queue.pushBack(u'\u13F4');
        } break;
        case u'\u13FD': {
            queue.pushBack(u'\u13F5');
        } break;
        case u'\u1C88': [[fallthrough]];
        case u'\uA64A': {
            queue.pushBack(u'\uA64B');
        } break;
        case u'\u1C90': {
            queue.pushBack(u'\u10D0');
        } break;
        case u'\u1C91': {
            queue.pushBack(u'\u10D1');
        } break;
        case u'\u1C92': {
            queue.pushBack(u'\u10D2');
        } break;
        case u'\u1C93': {
            queue.pushBack(u'\u10D3');
        } break;
        case u'\u1C94': {
            queue.pushBack(u'\u10D4');
        } break;
        case u'\u1C95': {
            queue.pushBack(u'\u10D5');
        } break;
        case u'\u1C96': {
            queue.pushBack(u'\u10D6');
        } break;
        case u'\u1C97': {
            queue.pushBack(u'\u10D7');
        } break;
        case u'\u1C98': {
            queue.pushBack(u'\u10D8');
        } break;
        case u'\u1C99': {
            queue.pushBack(u'\u10D9');
        } break;
        case u'\u1C9A': {
            queue.pushBack(u'\u10DA');
        } break;
        case u'\u1C9B': {
            queue.pushBack(u'\u10DB');
        } break;
        case u'\u1C9C': {
            queue.pushBack(u'\u10DC');
        } break;
        case u'\u1C9D': {
            queue.pushBack(u'\u10DD');
        } break;
        case u'\u1C9E': {
            queue.pushBack(u'\u10DE');
        } break;
        case u'\u1C9F': {
            queue.pushBack(u'\u10DF');
        } break;
        case u'\u1CA0': {
            queue.pushBack(u'\u10E0');
        } break;
        case u'\u1CA1': {
            queue.pushBack(u'\u10E1');
        } break;
        case u'\u1CA2': {
            queue.pushBack(u'\u10E2');
        } break;
        case u'\u1CA3': {
            queue.pushBack(u'\u10E3');
        } break;
        case u'\u1CA4': {
            queue.pushBack(u'\u10E4');
        } break;
        case u'\u1CA5': {
            queue.pushBack(u'\u10E5');
        } break;
        case u'\u1CA6': {
            queue.pushBack(u'\u10E6');
        } break;
        case u'\u1CA7': {
            queue.pushBack(u'\u10E7');
        } break;
        case u'\u1CA8': {
            queue.pushBack(u'\u10E8');
        } break;
        case u'\u1CA9': {
            queue.pushBack(u'\u10E9');
        } break;
        case u'\u1CAA': {
            queue.pushBack(u'\u10EA');
        } break;
        case u'\u1CAB': {
            queue.pushBack(u'\u10EB');
        } break;
        case u'\u1CAC': {
            queue.pushBack(u'\u10EC');
        } break;
        case u'\u1CAD': {
            queue.pushBack(u'\u10ED');
        } break;
        case u'\u1CAE': {
            queue.pushBack(u'\u10EE');
        } break;
        case u'\u1CAF': {
            queue.pushBack(u'\u10EF');
        } break;
        case u'\u1CB0': {
            queue.pushBack(u'\u10F0');
        } break;
        case u'\u1CB1': {
            queue.pushBack(u'\u10F1');
        } break;
        case u'\u1CB2': {
            queue.pushBack(u'\u10F2');
        } break;
        case u'\u1CB3': {
            queue.pushBack(u'\u10F3');
        } break;
        case u'\u1CB4': {
            queue.pushBack(u'\u10F4');
        } break;
        case u'\u1CB5': {
            queue.pushBack(u'\u10F5');
        } break;
        case u'\u1CB6': {
            queue.pushBack(u'\u10F6');
        } break;
        case u'\u1CB7': {
            queue.pushBack(u'\u10F7');
        } break;
        case u'\u1CB8': {
            queue.pushBack(u'\u10F8');
        } break;
        case u'\u1CB9': {
            queue.pushBack(u'\u10F9');
        } break;
        case u'\u1CBA': {
            queue.pushBack(u'\u10FA');
        } break;
        case u'\u1CBD': {
            queue.pushBack(u'\u10FD');
        } break;
        case u'\u1CBE': {
            queue.pushBack(u'\u10FE');
        } break;
        case u'\u1CBF': {
            queue.pushBack(u'\u10FF');
        } break;
        case u'\u1E00': {
            queue.pushBack(u'\u1E01');
        } break;
        case u'\u1E02': {
            queue.pushBack(u'\u1E03');
        } break;
        case u'\u1E04': {
            queue.pushBack(u'\u1E05');
        } break;
        case u'\u1E06': {
            queue.pushBack(u'\u1E07');
        } break;
        case u'\u1E08': {
            queue.pushBack(u'\u1E09');
        } break;
        case u'\u1E0A': {
            queue.pushBack(u'\u1E0B');
        } break;
        case u'\u1E0C': {
            queue.pushBack(u'\u1E0D');
        } break;
        case u'\u1E0E': {
            queue.pushBack(u'\u1E0F');
        } break;
        case u'\u1E10': {
            queue.pushBack(u'\u1E11');
        } break;
        case u'\u1E12': {
            queue.pushBack(u'\u1E13');
        } break;
        case u'\u1E14': {
            queue.pushBack(u'\u1E15');
        } break;
        case u'\u1E16': {
            queue.pushBack(u'\u1E17');
        } break;
        case u'\u1E18': {
            queue.pushBack(u'\u1E19');
        } break;
        case u'\u1E1A': {
            queue.pushBack(u'\u1E1B');
        } break;
        case u'\u1E1C': {
            queue.pushBack(u'\u1E1D');
        } break;
        case u'\u1E1E': {
            queue.pushBack(u'\u1E1F');
        } break;
        case u'\u1E20': {
            queue.pushBack(u'\u1E21');
        } break;
        case u'\u1E22': {
            queue.pushBack(u'\u1E23');
        } break;
        case u'\u1E24': {
            queue.pushBack(u'\u1E25');
        } break;
        case u'\u1E26': {
            queue.pushBack(u'\u1E27');
        } break;
        case u'\u1E28': {
            queue.pushBack(u'\u1E29');
        } break;
        case u'\u1E2A': {
            queue.pushBack(u'\u1E2B');
        } break;
        case u'\u1E2C': {
            queue.pushBack(u'\u1E2D');
        } break;
        case u'\u1E2E': {
            queue.pushBack(u'\u1E2F');
        } break;
        case u'\u1E30': {
            queue.pushBack(u'\u1E31');
        } break;
        case u'\u1E32': {
            queue.pushBack(u'\u1E33');
        } break;
        case u'\u1E34': {
            queue.pushBack(u'\u1E35');
        } break;
        case u'\u1E36': {
            queue.pushBack(u'\u1E37');
        } break;
        case u'\u1E38': {
            queue.pushBack(u'\u1E39');
        } break;
        case u'\u1E3A': {
            queue.pushBack(u'\u1E3B');
        } break;
        case u'\u1E3C': {
            queue.pushBack(u'\u1E3D');
        } break;
        case u'\u1E3E': {
            queue.pushBack(u'\u1E3F');
        } break;
        case u'\u1E40': {
            queue.pushBack(u'\u1E41');
        } break;
        case u'\u1E42': {
            queue.pushBack(u'\u1E43');
        } break;
        case u'\u1E44': {
            queue.pushBack(u'\u1E45');
        } break;
        case u'\u1E46': {
            queue.pushBack(u'\u1E47');
        } break;
        case u'\u1E48': {
            queue.pushBack(u'\u1E49');
        } break;
        case u'\u1E4A': {
            queue.pushBack(u'\u1E4B');
        } break;
        case u'\u1E4C': {
            queue.pushBack(u'\u1E4D');
        } break;
        case u'\u1E4E': {
            queue.pushBack(u'\u1E4F');
        } break;
        case u'\u1E50': {
            queue.pushBack(u'\u1E51');
        } break;
        case u'\u1E52': {
            queue.pushBack(u'\u1E53');
        } break;
        case u'\u1E54': {
            queue.pushBack(u'\u1E55');
        } break;
        case u'\u1E56': {
            queue.pushBack(u'\u1E57');
        } break;
        case u'\u1E58': {
            queue.pushBack(u'\u1E59');
        } break;
        case u'\u1E5A': {
            queue.pushBack(u'\u1E5B');
        } break;
        case u'\u1E5C': {
            queue.pushBack(u'\u1E5D');
        } break;
        case u'\u1E5E': {
            queue.pushBack(u'\u1E5F');
        } break;
        case u'\u1E60': [[fallthrough]];
        case u'\u1E9B': {
            queue.pushBack(u'\u1E61');
        } break;
        case u'\u1E62': {
            queue.pushBack(u'\u1E63');
        } break;
        case u'\u1E64': {
            queue.pushBack(u'\u1E65');
        } break;
        case u'\u1E66': {
            queue.pushBack(u'\u1E67');
        } break;
        case u'\u1E68': {
            queue.pushBack(u'\u1E69');
        } break;
        case u'\u1E6A': {
            queue.pushBack(u'\u1E6B');
        } break;
        case u'\u1E6C': {
            queue.pushBack(u'\u1E6D');
        } break;
        case u'\u1E6E': {
            queue.pushBack(u'\u1E6F');
        } break;
        case u'\u1E70': {
            queue.pushBack(u'\u1E71');
        } break;
        case u'\u1E72': {
            queue.pushBack(u'\u1E73');
        } break;
        case u'\u1E74': {
            queue.pushBack(u'\u1E75');
        } break;
        case u'\u1E76': {
            queue.pushBack(u'\u1E77');
        } break;
        case u'\u1E78': {
            queue.pushBack(u'\u1E79');
        } break;
        case u'\u1E7A': {
            queue.pushBack(u'\u1E7B');
        } break;
        case u'\u1E7C': {
            queue.pushBack(u'\u1E7D');
        } break;
        case u'\u1E7E': {
            queue.pushBack(u'\u1E7F');
        } break;
        case u'\u1E80': {
            queue.pushBack(u'\u1E81');
        } break;
        case u'\u1E82': {
            queue.pushBack(u'\u1E83');
        } break;
        case u'\u1E84': {
            queue.pushBack(u'\u1E85');
        } break;
        case u'\u1E86': {
            queue.pushBack(u'\u1E87');
        } break;
        case u'\u1E88': {
            queue.pushBack(u'\u1E89');
        } break;
        case u'\u1E8A': {
            queue.pushBack(u'\u1E8B');
        } break;
        case u'\u1E8C': {
            queue.pushBack(u'\u1E8D');
        } break;
        case u'\u1E8E': {
            queue.pushBack(u'\u1E8F');
        } break;
        case u'\u1E90': {
            queue.pushBack(u'\u1E91');
        } break;
        case u'\u1E92': {
            queue.pushBack(u'\u1E93');
        } break;
        case u'\u1E94': {
            queue.pushBack(u'\u1E95');
        } break;
        case u'\u1E96': {
            queue.pushBack(u'\u0068');
            queue.pushBack(u'\u0331');
        } break;
        case u'\u1E97': {
            queue.pushBack(u'\u0074');
            queue.pushBack(u'\u0308');
        } break;
        case u'\u1E98': {
            queue.pushBack(u'\u0077');
            queue.pushBack(u'\u030A');
        } break;
        case u'\u1E99': {
            queue.pushBack(u'\u0079');
            queue.pushBack(u'\u030A');
        } break;
        case u'\u1E9A': {
            queue.pushBack(u'\u0061');
            queue.pushBack(u'\u02BE');
        } break;
        case u'\u1EA0': {
            queue.pushBack(u'\u1EA1');
        } break;
        case u'\u1EA2': {
            queue.pushBack(u'\u1EA3');
        } break;
        case u'\u1EA4': {
            queue.pushBack(u'\u1EA5');
        } break;
        case u'\u1EA6': {
            queue.pushBack(u'\u1EA7');
        } break;
        case u'\u1EA8': {
            queue.pushBack(u'\u1EA9');
        } break;
        case u'\u1EAA': {
            queue.pushBack(u'\u1EAB');
        } break;
        case u'\u1EAC': {
            queue.pushBack(u'\u1EAD');
        } break;
        case u'\u1EAE': {
            queue.pushBack(u'\u1EAF');
        } break;
        case u'\u1EB0': {
            queue.pushBack(u'\u1EB1');
        } break;
        case u'\u1EB2': {
            queue.pushBack(u'\u1EB3');
        } break;
        case u'\u1EB4': {
            queue.pushBack(u'\u1EB5');
        } break;
        case u'\u1EB6': {
            queue.pushBack(u'\u1EB7');
        } break;
        case u'\u1EB8': {
            queue.pushBack(u'\u1EB9');
        } break;
        case u'\u1EBA': {
            queue.pushBack(u'\u1EBB');
        } break;
        case u'\u1EBC': {
            queue.pushBack(u'\u1EBD');
        } break;
        case u'\u1EBE': {
            queue.pushBack(u'\u1EBF');
        } break;
        case u'\u1EC0': {
            queue.pushBack(u'\u1EC1');
        } break;
        case u'\u1EC2': {
            queue.pushBack(u'\u1EC3');
        } break;
        case u'\u1EC4': {
            queue.pushBack(u'\u1EC5');
        } break;
        case u'\u1EC6': {
            queue.pushBack(u'\u1EC7');
        } break;
        case u'\u1EC8': {
            queue.pushBack(u'\u1EC9');
        } break;
        case u'\u1ECA': {
            queue.pushBack(u'\u1ECB');
        } break;
        case u'\u1ECC': {
            queue.pushBack(u'\u1ECD');
        } break;
        case u'\u1ECE': {
            queue.pushBack(u'\u1ECF');
        } break;
        case u'\u1ED0': {
            queue.pushBack(u'\u1ED1');
        } break;
        case u'\u1ED2': {
            queue.pushBack(u'\u1ED3');
        } break;
        case u'\u1ED4': {
            queue.pushBack(u'\u1ED5');
        } break;
        case u'\u1ED6': {
            queue.pushBack(u'\u1ED7');
        } break;
        case u'\u1ED8': {
            queue.pushBack(u'\u1ED9');
        } break;
        case u'\u1EDA': {
            queue.pushBack(u'\u1EDB');
        } break;
        case u'\u1EDC': {
            queue.pushBack(u'\u1EDD');
        } break;
        case u'\u1EDE': {
            queue.pushBack(u'\u1EDF');
        } break;
        case u'\u1EE0': {
            queue.pushBack(u'\u1EE1');
        } break;
        case u'\u1EE2': {
            queue.pushBack(u'\u1EE3');
        } break;
        case u'\u1EE4': {
            queue.pushBack(u'\u1EE5');
        } break;
        case u'\u1EE6': {
            queue.pushBack(u'\u1EE7');
        } break;
        case u'\u1EE8': {
            queue.pushBack(u'\u1EE9');
        } break;
        case u'\u1EEA': {
            queue.pushBack(u'\u1EEB');
        } break;
        case u'\u1EEC': {
            queue.pushBack(u'\u1EED');
        } break;
        case u'\u1EEE': {
            queue.pushBack(u'\u1EEF');
        } break;
        case u'\u1EF0': {
            queue.pushBack(u'\u1EF1');
        } break;
        case u'\u1EF2': {
            queue.pushBack(u'\u1EF3');
        } break;
        case u'\u1EF4': {
            queue.pushBack(u'\u1EF5');
        } break;
        case u'\u1EF6': {
            queue.pushBack(u'\u1EF7');
        } break;
        case u'\u1EF8': {
            queue.pushBack(u'\u1EF9');
        } break;
        case u'\u1EFA': {
            queue.pushBack(u'\u1EFB');
        } break;
        case u'\u1EFC': {
            queue.pushBack(u'\u1EFD');
        } break;
        case u'\u1EFE': {
            queue.pushBack(u'\u1EFF');
        } break;
        case u'\u1F08': {
            queue.pushBack(u'\u1F00');
        } break;
        case u'\u1F09': {
            queue.pushBack(u'\u1F01');
        } break;
        case u'\u1F0A': {
            queue.pushBack(u'\u1F02');
        } break;
        case u'\u1F0B': {
            queue.pushBack(u'\u1F03');
        } break;
        case u'\u1F0C': {
            queue.pushBack(u'\u1F04');
        } break;
        case u'\u1F0D': {
            queue.pushBack(u'\u1F05');
        } break;
        case u'\u1F0E': {
            queue.pushBack(u'\u1F06');
        } break;
        case u'\u1F0F': {
            queue.pushBack(u'\u1F07');
        } break;
        case u'\u1F18': {
            queue.pushBack(u'\u1F10');
        } break;
        case u'\u1F19': {
            queue.pushBack(u'\u1F11');
        } break;
        case u'\u1F1A': {
            queue.pushBack(u'\u1F12');
        } break;
        case u'\u1F1B': {
            queue.pushBack(u'\u1F13');
        } break;
        case u'\u1F1C': {
            queue.pushBack(u'\u1F14');
        } break;
        case u'\u1F1D': {
            queue.pushBack(u'\u1F15');
        } break;
        case u'\u1F28': {
            queue.pushBack(u'\u1F20');
        } break;
        case u'\u1F29': {
            queue.pushBack(u'\u1F21');
        } break;
        case u'\u1F2A': {
            queue.pushBack(u'\u1F22');
        } break;
        case u'\u1F2B': {
            queue.pushBack(u'\u1F23');
        } break;
        case u'\u1F2C': {
            queue.pushBack(u'\u1F24');
        } break;
        case u'\u1F2D': {
            queue.pushBack(u'\u1F25');
        } break;
        case u'\u1F2E': {
            queue.pushBack(u'\u1F26');
        } break;
        case u'\u1F2F': {
            queue.pushBack(u'\u1F27');
        } break;
        case u'\u1F38': {
            queue.pushBack(u'\u1F30');
        } break;
        case u'\u1F39': {
            queue.pushBack(u'\u1F31');
        } break;
        case u'\u1F3A': {
            queue.pushBack(u'\u1F32');
        } break;
        case u'\u1F3B': {
            queue.pushBack(u'\u1F33');
        } break;
        case u'\u1F3C': {
            queue.pushBack(u'\u1F34');
        } break;
        case u'\u1F3D': {
            queue.pushBack(u'\u1F35');
        } break;
        case u'\u1F3E': {
            queue.pushBack(u'\u1F36');
        } break;
        case u'\u1F3F': {
            queue.pushBack(u'\u1F37');
        } break;
        case u'\u1F48': {
            queue.pushBack(u'\u1F40');
        } break;
        case u'\u1F49': {
            queue.pushBack(u'\u1F41');
        } break;
        case u'\u1F4A': {
            queue.pushBack(u'\u1F42');
        } break;
        case u'\u1F4B': {
            queue.pushBack(u'\u1F43');
        } break;
        case u'\u1F4C': {
            queue.pushBack(u'\u1F44');
        } break;
        case u'\u1F4D': {
            queue.pushBack(u'\u1F45');
        } break;
        case u'\u1F50': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0313');
        } break;
        case u'\u1F52': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0313');
            queue.pushBack(u'\u0300');
        } break;
        case u'\u1F54': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0313');
            queue.pushBack(u'\u0301');
        } break;
        case u'\u1F56': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0313');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1F59': {
            queue.pushBack(u'\u1F51');
        } break;
        case u'\u1F5B': {
            queue.pushBack(u'\u1F53');
        } break;
        case u'\u1F5D': {
            queue.pushBack(u'\u1F55');
        } break;
        case u'\u1F5F': {
            queue.pushBack(u'\u1F57');
        } break;
        case u'\u1F68': {
            queue.pushBack(u'\u1F60');
        } break;
        case u'\u1F69': {
            queue.pushBack(u'\u1F61');
        } break;
        case u'\u1F6A': {
            queue.pushBack(u'\u1F62');
        } break;
        case u'\u1F6B': {
            queue.pushBack(u'\u1F63');
        } break;
        case u'\u1F6C': {
            queue.pushBack(u'\u1F64');
        } break;
        case u'\u1F6D': {
            queue.pushBack(u'\u1F65');
        } break;
        case u'\u1F6E': {
            queue.pushBack(u'\u1F66');
        } break;
        case u'\u1F6F': {
            queue.pushBack(u'\u1F67');
        } break;
        case u'\u1F80': [[fallthrough]];
        case u'\u1F88': {
            queue.pushBack(u'\u1F00');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F81': [[fallthrough]];
        case u'\u1F89': {
            queue.pushBack(u'\u1F01');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F82': [[fallthrough]];
        case u'\u1F8A': {
            queue.pushBack(u'\u1F02');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F83': [[fallthrough]];
        case u'\u1F8B': {
            queue.pushBack(u'\u1F03');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F84': [[fallthrough]];
        case u'\u1F8C': {
            queue.pushBack(u'\u1F04');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F85': [[fallthrough]];
        case u'\u1F8D': {
            queue.pushBack(u'\u1F05');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F86': [[fallthrough]];
        case u'\u1F8E': {
            queue.pushBack(u'\u1F06');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F87': [[fallthrough]];
        case u'\u1F8F': {
            queue.pushBack(u'\u1F07');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F90': [[fallthrough]];
        case u'\u1F98': {
            queue.pushBack(u'\u1F20');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F91': [[fallthrough]];
        case u'\u1F99': {
            queue.pushBack(u'\u1F21');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F92': [[fallthrough]];
        case u'\u1F9A': {
            queue.pushBack(u'\u1F22');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F93': [[fallthrough]];
        case u'\u1F9B': {
            queue.pushBack(u'\u1F23');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F94': [[fallthrough]];
        case u'\u1F9C': {
            queue.pushBack(u'\u1F24');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F95': [[fallthrough]];
        case u'\u1F9D': {
            queue.pushBack(u'\u1F25');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F96': [[fallthrough]];
        case u'\u1F9E': {
            queue.pushBack(u'\u1F26');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1F97': [[fallthrough]];
        case u'\u1F9F': {
            queue.pushBack(u'\u1F27');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA0': [[fallthrough]];
        case u'\u1FA8': {
            queue.pushBack(u'\u1F60');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA1': [[fallthrough]];
        case u'\u1FA9': {
            queue.pushBack(u'\u1F61');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA2': [[fallthrough]];
        case u'\u1FAA': {
            queue.pushBack(u'\u1F62');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA3': [[fallthrough]];
        case u'\u1FAB': {
            queue.pushBack(u'\u1F63');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA4': [[fallthrough]];
        case u'\u1FAC': {
            queue.pushBack(u'\u1F64');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA5': [[fallthrough]];
        case u'\u1FAD': {
            queue.pushBack(u'\u1F65');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA6': [[fallthrough]];
        case u'\u1FAE': {
            queue.pushBack(u'\u1F66');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FA7': [[fallthrough]];
        case u'\u1FAF': {
            queue.pushBack(u'\u1F67');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FB2': {
            queue.pushBack(u'\u1F70');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FB3': [[fallthrough]];
        case u'\u1FBC': {
            queue.pushBack(u'\u03B1');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FB4': {
            queue.pushBack(u'\u03AC');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FB6': {
            queue.pushBack(u'\u03B1');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FB7': {
            queue.pushBack(u'\u03B1');
            queue.pushBack(u'\u0342');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FB8': {
            queue.pushBack(u'\u1FB0');
        } break;
        case u'\u1FB9': {
            queue.pushBack(u'\u1FB1');
        } break;
        case u'\u1FBA': {
            queue.pushBack(u'\u1F70');
        } break;
        case u'\u1FBB': {
            queue.pushBack(u'\u1F71');
        } break;
        case u'\u1FC2': {
            queue.pushBack(u'\u1F74');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FC3': [[fallthrough]];
        case u'\u1FCC': {
            queue.pushBack(u'\u03B7');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FC4': {
            queue.pushBack(u'\u03AE');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FC6': {
            queue.pushBack(u'\u03B7');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FC7': {
            queue.pushBack(u'\u03B7');
            queue.pushBack(u'\u0342');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FC8': {
            queue.pushBack(u'\u1F72');
        } break;
        case u'\u1FC9': {
            queue.pushBack(u'\u1F73');
        } break;
        case u'\u1FCA': {
            queue.pushBack(u'\u1F74');
        } break;
        case u'\u1FCB': {
            queue.pushBack(u'\u1F75');
        } break;
        case u'\u1FD2': {
            queue.pushBack(u'\u03B9');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0300');
        } break;
        case u'\u1FD6': {
            queue.pushBack(u'\u03B9');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FD7': {
            queue.pushBack(u'\u03B9');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FD8': {
            queue.pushBack(u'\u1FD0');
        } break;
        case u'\u1FD9': {
            queue.pushBack(u'\u1FD1');
        } break;
        case u'\u1FDA': {
            queue.pushBack(u'\u1F76');
        } break;
        case u'\u1FDB': {
            queue.pushBack(u'\u1F77');
        } break;
        case u'\u1FE2': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0300');
        } break;
        case u'\u1FE4': {
            queue.pushBack(u'\u03C1');
            queue.pushBack(u'\u0313');
        } break;
        case u'\u1FE6': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FE7': {
            queue.pushBack(u'\u03C5');
            queue.pushBack(u'\u0308');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FE8': {
            queue.pushBack(u'\u1FE0');
        } break;
        case u'\u1FE9': {
            queue.pushBack(u'\u1FE1');
        } break;
        case u'\u1FEA': {
            queue.pushBack(u'\u1F7A');
        } break;
        case u'\u1FEB': {
            queue.pushBack(u'\u1F7B');
        } break;
        case u'\u1FEC': {
            queue.pushBack(u'\u1FE5');
        } break;
        case u'\u1FF2': {
            queue.pushBack(u'\u1F7C');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FF3': [[fallthrough]];
        case u'\u1FFC': {
            queue.pushBack(u'\u03C9');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FF4': {
            queue.pushBack(u'\u03CE');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FF6': {
            queue.pushBack(u'\u03C9');
            queue.pushBack(u'\u0342');
        } break;
        case u'\u1FF7': {
            queue.pushBack(u'\u03C9');
            queue.pushBack(u'\u0342');
            queue.pushBack(u'\u03B9');
        } break;
        case u'\u1FF8': {
            queue.pushBack(u'\u1F78');
        } break;
        case u'\u1FF9': {
            queue.pushBack(u'\u1F79');
        } break;
        case u'\u1FFA': {
            queue.pushBack(u'\u1F7C');
        } break;
        case u'\u1FFB': {
            queue.pushBack(u'\u1F7D');
        } break;
        case u'\u03A9': [[fallthrough]];
        case u'\u2126': {
            queue.pushBack(u'\u03C9');
        } break;
        case u'\u2132': {
            queue.pushBack(u'\u214E');
        } break;
        case u'\u2160': {
            queue.pushBack(u'\u2170');
        } break;
        case u'\u2161': {
            queue.pushBack(u'\u2171');
        } break;
        case u'\u2162': {
            queue.pushBack(u'\u2172');
        } break;
        case u'\u2163': {
            queue.pushBack(u'\u2173');
        } break;
        case u'\u2164': {
            queue.pushBack(u'\u2174');
        } break;
        case u'\u2165': {
            queue.pushBack(u'\u2175');
        } break;
        case u'\u2166': {
            queue.pushBack(u'\u2176');
        } break;
        case u'\u2167': {
            queue.pushBack(u'\u2177');
        } break;
        case u'\u2168': {
            queue.pushBack(u'\u2178');
        } break;
        case u'\u2169': {
            queue.pushBack(u'\u2179');
        } break;
        case u'\u216A': {
            queue.pushBack(u'\u217A');
        } break;
        case u'\u216B': {
            queue.pushBack(u'\u217B');
        } break;
        case u'\u216C': {
            queue.pushBack(u'\u217C');
        } break;
        case u'\u216D': {
            queue.pushBack(u'\u217D');
        } break;
        case u'\u216E': {
            queue.pushBack(u'\u217E');
        } break;
        case u'\u216F': {
            queue.pushBack(u'\u217F');
        } break;
        case u'\u2183': {
            queue.pushBack(u'\u2184');
        } break;
        case u'\u24B6': {
            queue.pushBack(u'\u24D0');
        } break;
        case u'\u24B7': {
            queue.pushBack(u'\u24D1');
        } break;
        case u'\u24B8': {
            queue.pushBack(u'\u24D2');
        } break;
        case u'\u24B9': {
            queue.pushBack(u'\u24D3');
        } break;
        case u'\u24BA': {
            queue.pushBack(u'\u24D4');
        } break;
        case u'\u24BB': {
            queue.pushBack(u'\u24D5');
        } break;
        case u'\u24BC': {
            queue.pushBack(u'\u24D6');
        } break;
        case u'\u24BD': {
            queue.pushBack(u'\u24D7');
        } break;
        case u'\u24BE': {
            queue.pushBack(u'\u24D8');
        } break;
        case u'\u24BF': {
            queue.pushBack(u'\u24D9');
        } break;
        case u'\u24C0': {
            queue.pushBack(u'\u24DA');
        } break;
        case u'\u24C1': {
            queue.pushBack(u'\u24DB');
        } break;
        case u'\u24C2': {
            queue.pushBack(u'\u24DC');
        } break;
        case u'\u24C3': {
            queue.pushBack(u'\u24DD');
        } break;
        case u'\u24C4': {
            queue.pushBack(u'\u24DE');
        } break;
        case u'\u24C5': {
            queue.pushBack(u'\u24DF');
        } break;
        case u'\u24C6': {
            queue.pushBack(u'\u24E0');
        } break;
        case u'\u24C7': {
            queue.pushBack(u'\u24E1');
        } break;
        case u'\u24C8': {
            queue.pushBack(u'\u24E2');
        } break;
        case u'\u24C9': {
            queue.pushBack(u'\u24E3');
        } break;
        case u'\u24CA': {
            queue.pushBack(u'\u24E4');
        } break;
        case u'\u24CB': {
            queue.pushBack(u'\u24E5');
        } break;
        case u'\u24CC': {
            queue.pushBack(u'\u24E6');
        } break;
        case u'\u24CD': {
            queue.pushBack(u'\u24E7');
        } break;
        case u'\u24CE': {
            queue.pushBack(u'\u24E8');
        } break;
        case u'\u24CF': {
            queue.pushBack(u'\u24E9');
        } break;
        case u'\u2C00': {
            queue.pushBack(u'\u2C30');
        } break;
        case u'\u2C01': {
            queue.pushBack(u'\u2C31');
        } break;
        case u'\u2C02': {
            queue.pushBack(u'\u2C32');
        } break;
        case u'\u2C03': {
            queue.pushBack(u'\u2C33');
        } break;
        case u'\u2C04': {
            queue.pushBack(u'\u2C34');
        } break;
        case u'\u2C05': {
            queue.pushBack(u'\u2C35');
        } break;
        case u'\u2C06': {
            queue.pushBack(u'\u2C36');
        } break;
        case u'\u2C07': {
            queue.pushBack(u'\u2C37');
        } break;
        case u'\u2C08': {
            queue.pushBack(u'\u2C38');
        } break;
        case u'\u2C09': {
            queue.pushBack(u'\u2C39');
        } break;
        case u'\u2C0A': {
            queue.pushBack(u'\u2C3A');
        } break;
        case u'\u2C0B': {
            queue.pushBack(u'\u2C3B');
        } break;
        case u'\u2C0C': {
            queue.pushBack(u'\u2C3C');
        } break;
        case u'\u2C0D': {
            queue.pushBack(u'\u2C3D');
        } break;
        case u'\u2C0E': {
            queue.pushBack(u'\u2C3E');
        } break;
        case u'\u2C0F': {
            queue.pushBack(u'\u2C3F');
        } break;
        case u'\u2C10': {
            queue.pushBack(u'\u2C40');
        } break;
        case u'\u2C11': {
            queue.pushBack(u'\u2C41');
        } break;
        case u'\u2C12': {
            queue.pushBack(u'\u2C42');
        } break;
        case u'\u2C13': {
            queue.pushBack(u'\u2C43');
        } break;
        case u'\u2C14': {
            queue.pushBack(u'\u2C44');
        } break;
        case u'\u2C15': {
            queue.pushBack(u'\u2C45');
        } break;
        case u'\u2C16': {
            queue.pushBack(u'\u2C46');
        } break;
        case u'\u2C17': {
            queue.pushBack(u'\u2C47');
        } break;
        case u'\u2C18': {
            queue.pushBack(u'\u2C48');
        } break;
        case u'\u2C19': {
            queue.pushBack(u'\u2C49');
        } break;
        case u'\u2C1A': {
            queue.pushBack(u'\u2C4A');
        } break;
        case u'\u2C1B': {
            queue.pushBack(u'\u2C4B');
        } break;
        case u'\u2C1C': {
            queue.pushBack(u'\u2C4C');
        } break;
        case u'\u2C1D': {
            queue.pushBack(u'\u2C4D');
        } break;
        case u'\u2C1E': {
            queue.pushBack(u'\u2C4E');
        } break;
        case u'\u2C1F': {
            queue.pushBack(u'\u2C4F');
        } break;
        case u'\u2C20': {
            queue.pushBack(u'\u2C50');
        } break;
        case u'\u2C21': {
            queue.pushBack(u'\u2C51');
        } break;
        case u'\u2C22': {
            queue.pushBack(u'\u2C52');
        } break;
        case u'\u2C23': {
            queue.pushBack(u'\u2C53');
        } break;
        case u'\u2C24': {
            queue.pushBack(u'\u2C54');
        } break;
        case u'\u2C25': {
            queue.pushBack(u'\u2C55');
        } break;
        case u'\u2C26': {
            queue.pushBack(u'\u2C56');
        } break;
        case u'\u2C27': {
            queue.pushBack(u'\u2C57');
        } break;
        case u'\u2C28': {
            queue.pushBack(u'\u2C58');
        } break;
        case u'\u2C29': {
            queue.pushBack(u'\u2C59');
        } break;
        case u'\u2C2A': {
            queue.pushBack(u'\u2C5A');
        } break;
        case u'\u2C2B': {
            queue.pushBack(u'\u2C5B');
        } break;
        case u'\u2C2C': {
            queue.pushBack(u'\u2C5C');
        } break;
        case u'\u2C2D': {
            queue.pushBack(u'\u2C5D');
        } break;
        case u'\u2C2E': {
            queue.pushBack(u'\u2C5E');
        } break;
        case u'\u2C2F': {
            queue.pushBack(u'\u2C5F');
        } break;
        case u'\u2C60': {
            queue.pushBack(u'\u2C61');
        } break;
        case u'\u2C62': {
            queue.pushBack(u'\u026B');
        } break;
        case u'\u2C63': {
            queue.pushBack(u'\u1D7D');
        } break;
        case u'\u2C64': {
            queue.pushBack(u'\u027D');
        } break;
        case u'\u2C67': {
            queue.pushBack(u'\u2C68');
        } break;
        case u'\u2C69': {
            queue.pushBack(u'\u2C6A');
        } break;
        case u'\u2C6B': {
            queue.pushBack(u'\u2C6C');
        } break;
        case u'\u2C6D': {
            queue.pushBack(u'\u0251');
        } break;
        case u'\u2C6E': {
            queue.pushBack(u'\u0271');
        } break;
        case u'\u2C6F': {
            queue.pushBack(u'\u0250');
        } break;
        case u'\u2C70': {
            queue.pushBack(u'\u0252');
        } break;
        case u'\u2C72': {
            queue.pushBack(u'\u2C73');
        } break;
        case u'\u2C75': {
            queue.pushBack(u'\u2C76');
        } break;
        case u'\u2C7E': {
            queue.pushBack(u'\u023F');
        } break;
        case u'\u2C7F': {
            queue.pushBack(u'\u0240');
        } break;
        case u'\u2C80': {
            queue.pushBack(u'\u2C81');
        } break;
        case u'\u2C82': {
            queue.pushBack(u'\u2C83');
        } break;
        case u'\u2C84': {
            queue.pushBack(u'\u2C85');
        } break;
        case u'\u2C86': {
            queue.pushBack(u'\u2C87');
        } break;
        case u'\u2C88': {
            queue.pushBack(u'\u2C89');
        } break;
        case u'\u2C8A': {
            queue.pushBack(u'\u2C8B');
        } break;
        case u'\u2C8C': {
            queue.pushBack(u'\u2C8D');
        } break;
        case u'\u2C8E': {
            queue.pushBack(u'\u2C8F');
        } break;
        case u'\u2C90': {
            queue.pushBack(u'\u2C91');
        } break;
        case u'\u2C92': {
            queue.pushBack(u'\u2C93');
        } break;
        case u'\u2C94': {
            queue.pushBack(u'\u2C95');
        } break;
        case u'\u2C96': {
            queue.pushBack(u'\u2C97');
        } break;
        case u'\u2C98': {
            queue.pushBack(u'\u2C99');
        } break;
        case u'\u2C9A': {
            queue.pushBack(u'\u2C9B');
        } break;
        case u'\u2C9C': {
            queue.pushBack(u'\u2C9D');
        } break;
        case u'\u2C9E': {
            queue.pushBack(u'\u2C9F');
        } break;
        case u'\u2CA0': {
            queue.pushBack(u'\u2CA1');
        } break;
        case u'\u2CA2': {
            queue.pushBack(u'\u2CA3');
        } break;
        case u'\u2CA4': {
            queue.pushBack(u'\u2CA5');
        } break;
        case u'\u2CA6': {
            queue.pushBack(u'\u2CA7');
        } break;
        case u'\u2CA8': {
            queue.pushBack(u'\u2CA9');
        } break;
        case u'\u2CAA': {
            queue.pushBack(u'\u2CAB');
        } break;
        case u'\u2CAC': {
            queue.pushBack(u'\u2CAD');
        } break;
        case u'\u2CAE': {
            queue.pushBack(u'\u2CAF');
        } break;
        case u'\u2CB0': {
            queue.pushBack(u'\u2CB1');
        } break;
        case u'\u2CB2': {
            queue.pushBack(u'\u2CB3');
        } break;
        case u'\u2CB4': {
            queue.pushBack(u'\u2CB5');
        } break;
        case u'\u2CB6': {
            queue.pushBack(u'\u2CB7');
        } break;
        case u'\u2CB8': {
            queue.pushBack(u'\u2CB9');
        } break;
        case u'\u2CBA': {
            queue.pushBack(u'\u2CBB');
        } break;
        case u'\u2CBC': {
            queue.pushBack(u'\u2CBD');
        } break;
        case u'\u2CBE': {
            queue.pushBack(u'\u2CBF');
        } break;
        case u'\u2CC0': {
            queue.pushBack(u'\u2CC1');
        } break;
        case u'\u2CC2': {
            queue.pushBack(u'\u2CC3');
        } break;
        case u'\u2CC4': {
            queue.pushBack(u'\u2CC5');
        } break;
        case u'\u2CC6': {
            queue.pushBack(u'\u2CC7');
        } break;
        case u'\u2CC8': {
            queue.pushBack(u'\u2CC9');
        } break;
        case u'\u2CCA': {
            queue.pushBack(u'\u2CCB');
        } break;
        case u'\u2CCC': {
            queue.pushBack(u'\u2CCD');
        } break;
        case u'\u2CCE': {
            queue.pushBack(u'\u2CCF');
        } break;
        case u'\u2CD0': {
            queue.pushBack(u'\u2CD1');
        } break;
        case u'\u2CD2': {
            queue.pushBack(u'\u2CD3');
        } break;
        case u'\u2CD4': {
            queue.pushBack(u'\u2CD5');
        } break;
        case u'\u2CD6': {
            queue.pushBack(u'\u2CD7');
        } break;
        case u'\u2CD8': {
            queue.pushBack(u'\u2CD9');
        } break;
        case u'\u2CDA': {
            queue.pushBack(u'\u2CDB');
        } break;
        case u'\u2CDC': {
            queue.pushBack(u'\u2CDD');
        } break;
        case u'\u2CDE': {
            queue.pushBack(u'\u2CDF');
        } break;
        case u'\u2CE0': {
            queue.pushBack(u'\u2CE1');
        } break;
        case u'\u2CE2': {
            queue.pushBack(u'\u2CE3');
        } break;
        case u'\u2CEB': {
            queue.pushBack(u'\u2CEC');
        } break;
        case u'\u2CED': {
            queue.pushBack(u'\u2CEE');
        } break;
        case u'\u2CF2': {
            queue.pushBack(u'\u2CF3');
        } break;
        case u'\uA640': {
            queue.pushBack(u'\uA641');
        } break;
        case u'\uA642': {
            queue.pushBack(u'\uA643');
        } break;
        case u'\uA644': {
            queue.pushBack(u'\uA645');
        } break;
        case u'\uA646': {
            queue.pushBack(u'\uA647');
        } break;
        case u'\uA648': {
            queue.pushBack(u'\uA649');
        } break;
        case u'\uA64C': {
            queue.pushBack(u'\uA64D');
        } break;
        case u'\uA64E': {
            queue.pushBack(u'\uA64F');
        } break;
        case u'\uA650': {
            queue.pushBack(u'\uA651');
        } break;
        case u'\uA652': {
            queue.pushBack(u'\uA653');
        } break;
        case u'\uA654': {
            queue.pushBack(u'\uA655');
        } break;
        case u'\uA656': {
            queue.pushBack(u'\uA657');
        } break;
        case u'\uA658': {
            queue.pushBack(u'\uA659');
        } break;
        case u'\uA65A': {
            queue.pushBack(u'\uA65B');
        } break;
        case u'\uA65C': {
            queue.pushBack(u'\uA65D');
        } break;
        case u'\uA65E': {
            queue.pushBack(u'\uA65F');
        } break;
        case u'\uA660': {
            queue.pushBack(u'\uA661');
        } break;
        case u'\uA662': {
            queue.pushBack(u'\uA663');
        } break;
        case u'\uA664': {
            queue.pushBack(u'\uA665');
        } break;
        case u'\uA666': {
            queue.pushBack(u'\uA667');
        } break;
        case u'\uA668': {
            queue.pushBack(u'\uA669');
        } break;
        case u'\uA66A': {
            queue.pushBack(u'\uA66B');
        } break;
        case u'\uA66C': {
            queue.pushBack(u'\uA66D');
        } break;
        case u'\uA680': {
            queue.pushBack(u'\uA681');
        } break;
        case u'\uA682': {
            queue.pushBack(u'\uA683');
        } break;
        case u'\uA684': {
            queue.pushBack(u'\uA685');
        } break;
        case u'\uA686': {
            queue.pushBack(u'\uA687');
        } break;
        case u'\uA688': {
            queue.pushBack(u'\uA689');
        } break;
        case u'\uA68A': {
            queue.pushBack(u'\uA68B');
        } break;
        case u'\uA68C': {
            queue.pushBack(u'\uA68D');
        } break;
        case u'\uA68E': {
            queue.pushBack(u'\uA68F');
        } break;
        case u'\uA690': {
            queue.pushBack(u'\uA691');
        } break;
        case u'\uA692': {
            queue.pushBack(u'\uA693');
        } break;
        case u'\uA694': {
            queue.pushBack(u'\uA695');
        } break;
        case u'\uA696': {
            queue.pushBack(u'\uA697');
        } break;
        case u'\uA698': {
            queue.pushBack(u'\uA699');
        } break;
        case u'\uA69A': {
            queue.pushBack(u'\uA69B');
        } break;
        case u'\uA722': {
            queue.pushBack(u'\uA723');
        } break;
        case u'\uA724': {
            queue.pushBack(u'\uA725');
        } break;
        case u'\uA726': {
            queue.pushBack(u'\uA727');
        } break;
        case u'\uA728': {
            queue.pushBack(u'\uA729');
        } break;
        case u'\uA72A': {
            queue.pushBack(u'\uA72B');
        } break;
        case u'\uA72C': {
            queue.pushBack(u'\uA72D');
        } break;
        case u'\uA72E': {
            queue.pushBack(u'\uA72F');
        } break;
        case u'\uA732': {
            queue.pushBack(u'\uA733');
        } break;
        case u'\uA734': {
            queue.pushBack(u'\uA735');
        } break;
        case u'\uA736': {
            queue.pushBack(u'\uA737');
        } break;
        case u'\uA738': {
            queue.pushBack(u'\uA739');
        } break;
        case u'\uA73A': {
            queue.pushBack(u'\uA73B');
        } break;
        case u'\uA73C': {
            queue.pushBack(u'\uA73D');
        } break;
        case u'\uA73E': {
            queue.pushBack(u'\uA73F');
        } break;
        case u'\uA740': {
            queue.pushBack(u'\uA741');
        } break;
        case u'\uA742': {
            queue.pushBack(u'\uA743');
        } break;
        case u'\uA744': {
            queue.pushBack(u'\uA745');
        } break;
        case u'\uA746': {
            queue.pushBack(u'\uA747');
        } break;
        case u'\uA748': {
            queue.pushBack(u'\uA749');
        } break;
        case u'\uA74A': {
            queue.pushBack(u'\uA74B');
        } break;
        case u'\uA74C': {
            queue.pushBack(u'\uA74D');
        } break;
        case u'\uA74E': {
            queue.pushBack(u'\uA74F');
        } break;
        case u'\uA750': {
            queue.pushBack(u'\uA751');
        } break;
        case u'\uA752': {
            queue.pushBack(u'\uA753');
        } break;
        case u'\uA754': {
            queue.pushBack(u'\uA755');
        } break;
        case u'\uA756': {
            queue.pushBack(u'\uA757');
        } break;
        case u'\uA758': {
            queue.pushBack(u'\uA759');
        } break;
        case u'\uA75A': {
            queue.pushBack(u'\uA75B');
        } break;
        case u'\uA75C': {
            queue.pushBack(u'\uA75D');
        } break;
        case u'\uA75E': {
            queue.pushBack(u'\uA75F');
        } break;
        case u'\uA760': {
            queue.pushBack(u'\uA761');
        } break;
        case u'\uA762': {
            queue.pushBack(u'\uA763');
        } break;
        case u'\uA764': {
            queue.pushBack(u'\uA765');
        } break;
        case u'\uA766': {
            queue.pushBack(u'\uA767');
        } break;
        case u'\uA768': {
            queue.pushBack(u'\uA769');
        } break;
        case u'\uA76A': {
            queue.pushBack(u'\uA76B');
        } break;
        case u'\uA76C': {
            queue.pushBack(u'\uA76D');
        } break;
        case u'\uA76E': {
            queue.pushBack(u'\uA76F');
        } break;
        case u'\uA779': {
            queue.pushBack(u'\uA77A');
        } break;
        case u'\uA77B': {
            queue.pushBack(u'\uA77C');
        } break;
        case u'\uA77D': {
            queue.pushBack(u'\u1D79');
        } break;
        case u'\uA77E': {
            queue.pushBack(u'\uA77F');
        } break;
        case u'\uA780': {
            queue.pushBack(u'\uA781');
        } break;
        case u'\uA782': {
            queue.pushBack(u'\uA783');
        } break;
        case u'\uA784': {
            queue.pushBack(u'\uA785');
        } break;
        case u'\uA786': {
            queue.pushBack(u'\uA787');
        } break;
        case u'\uA78B': {
            queue.pushBack(u'\uA78C');
        } break;
        case u'\uA78D': {
            queue.pushBack(u'\u0265');
        } break;
        case u'\uA790': {
            queue.pushBack(u'\uA791');
        } break;
        case u'\uA792': {
            queue.pushBack(u'\uA793');
        } break;
        case u'\uA796': {
            queue.pushBack(u'\uA797');
        } break;
        case u'\uA798': {
            queue.pushBack(u'\uA799');
        } break;
        case u'\uA79A': {
            queue.pushBack(u'\uA79B');
        } break;
        case u'\uA79C': {
            queue.pushBack(u'\uA79D');
        } break;
        case u'\uA79E': {
            queue.pushBack(u'\uA79F');
        } break;
        case u'\uA7A0': {
            queue.pushBack(u'\uA7A1');
        } break;
        case u'\uA7A2': {
            queue.pushBack(u'\uA7A3');
        } break;
        case u'\uA7A4': {
            queue.pushBack(u'\uA7A5');
        } break;
        case u'\uA7A6': {
            queue.pushBack(u'\uA7A7');
        } break;
        case u'\uA7A8': {
            queue.pushBack(u'\uA7A9');
        } break;
        case u'\uA7AA': {
            queue.pushBack(u'\u0266');
        } break;
        case u'\uA7AB': {
            queue.pushBack(u'\u025C');
        } break;
        case u'\uA7AC': {
            queue.pushBack(u'\u0261');
        } break;
        case u'\uA7AD': {
            queue.pushBack(u'\u026C');
        } break;
        case u'\uA7AE': {
            queue.pushBack(u'\u026A');
        } break;
        case u'\uA7B0': {
            queue.pushBack(u'\u029E');
        } break;
        case u'\uA7B1': {
            queue.pushBack(u'\u0287');
        } break;
        case u'\uA7B2': {
            queue.pushBack(u'\u029D');
        } break;
        case u'\uA7B3': {
            queue.pushBack(u'\uAB53');
        } break;
        case u'\uA7B4': {
            queue.pushBack(u'\uA7B5');
        } break;
        case u'\uA7B6': {
            queue.pushBack(u'\uA7B7');
        } break;
        case u'\uA7B8': {
            queue.pushBack(u'\uA7B9');
        } break;
        case u'\uA7BA': {
            queue.pushBack(u'\uA7BB');
        } break;
        case u'\uA7BC': {
            queue.pushBack(u'\uA7BD');
        } break;
        case u'\uA7BE': {
            queue.pushBack(u'\uA7BF');
        } break;
        case u'\uA7C0': {
            queue.pushBack(u'\uA7C1');
        } break;
        case u'\uA7C2': {
            queue.pushBack(u'\uA7C3');
        } break;
        case u'\uA7C4': {
            queue.pushBack(u'\uA794');
        } break;
        case u'\uA7C5': {
            queue.pushBack(u'\u0282');
        } break;
        case u'\uA7C6': {
            queue.pushBack(u'\u1D8E');
        } break;
        case u'\uA7C7': {
            queue.pushBack(u'\uA7C8');
        } break;
        case u'\uA7C9': {
            queue.pushBack(u'\uA7CA');
        } break;
        case u'\uA7D0': {
            queue.pushBack(u'\uA7D1');
        } break;
        case u'\uA7D6': {
            queue.pushBack(u'\uA7D7');
        } break;
        case u'\uA7D8': {
            queue.pushBack(u'\uA7D9');
        } break;
        case u'\uA7F5': {
            queue.pushBack(u'\uA7F6');
        } break;
        case u'\uAB70': {
            queue.pushBack(u'\u13A0');
        } break;
        case u'\uAB71': {
            queue.pushBack(u'\u13A1');
        } break;
        case u'\uAB72': {
            queue.pushBack(u'\u13A2');
        } break;
        case u'\uAB73': {
            queue.pushBack(u'\u13A3');
        } break;
        case u'\uAB74': {
            queue.pushBack(u'\u13A4');
        } break;
        case u'\uAB75': {
            queue.pushBack(u'\u13A5');
        } break;
        case u'\uAB76': {
            queue.pushBack(u'\u13A6');
        } break;
        case u'\uAB77': {
            queue.pushBack(u'\u13A7');
        } break;
        case u'\uAB78': {
            queue.pushBack(u'\u13A8');
        } break;
        case u'\uAB79': {
            queue.pushBack(u'\u13A9');
        } break;
        case u'\uAB7A': {
            queue.pushBack(u'\u13AA');
        } break;
        case u'\uAB7B': {
            queue.pushBack(u'\u13AB');
        } break;
        case u'\uAB7C': {
            queue.pushBack(u'\u13AC');
        } break;
        case u'\uAB7D': {
            queue.pushBack(u'\u13AD');
        } break;
        case u'\uAB7E': {
            queue.pushBack(u'\u13AE');
        } break;
        case u'\uAB7F': {
            queue.pushBack(u'\u13AF');
        } break;
        case u'\uAB80': {
            queue.pushBack(u'\u13B0');
        } break;
        case u'\uAB81': {
            queue.pushBack(u'\u13B1');
        } break;
        case u'\uAB82': {
            queue.pushBack(u'\u13B2');
        } break;
        case u'\uAB83': {
            queue.pushBack(u'\u13B3');
        } break;
        case u'\uAB84': {
            queue.pushBack(u'\u13B4');
        } break;
        case u'\uAB85': {
            queue.pushBack(u'\u13B5');
        } break;
        case u'\uAB86': {
            queue.pushBack(u'\u13B6');
        } break;
        case u'\uAB87': {
            queue.pushBack(u'\u13B7');
        } break;
        case u'\uAB88': {
            queue.pushBack(u'\u13B8');
        } break;
        case u'\uAB89': {
            queue.pushBack(u'\u13B9');
        } break;
        case u'\uAB8A': {
            queue.pushBack(u'\u13BA');
        } break;
        case u'\uAB8B': {
            queue.pushBack(u'\u13BB');
        } break;
        case u'\uAB8C': {
            queue.pushBack(u'\u13BC');
        } break;
        case u'\uAB8D': {
            queue.pushBack(u'\u13BD');
        } break;
        case u'\uAB8E': {
            queue.pushBack(u'\u13BE');
        } break;
        case u'\uAB8F': {
            queue.pushBack(u'\u13BF');
        } break;
        case u'\uAB90': {
            queue.pushBack(u'\u13C0');
        } break;
        case u'\uAB91': {
            queue.pushBack(u'\u13C1');
        } break;
        case u'\uAB92': {
            queue.pushBack(u'\u13C2');
        } break;
        case u'\uAB93': {
            queue.pushBack(u'\u13C3');
        } break;
        case u'\uAB94': {
            queue.pushBack(u'\u13C4');
        } break;
        case u'\uAB95': {
            queue.pushBack(u'\u13C5');
        } break;
        case u'\uAB96': {
            queue.pushBack(u'\u13C6');
        } break;
        case u'\uAB97': {
            queue.pushBack(u'\u13C7');
        } break;
        case u'\uAB98': {
            queue.pushBack(u'\u13C8');
        } break;
        case u'\uAB99': {
            queue.pushBack(u'\u13C9');
        } break;
        case u'\uAB9A': {
            queue.pushBack(u'\u13CA');
        } break;
        case u'\uAB9B': {
            queue.pushBack(u'\u13CB');
        } break;
        case u'\uAB9C': {
            queue.pushBack(u'\u13CC');
        } break;
        case u'\uAB9D': {
            queue.pushBack(u'\u13CD');
        } break;
        case u'\uAB9E': {
            queue.pushBack(u'\u13CE');
        } break;
        case u'\uAB9F': {
            queue.pushBack(u'\u13CF');
        } break;
        case u'\uABA0': {
            queue.pushBack(u'\u13D0');
        } break;
        case u'\uABA1': {
            queue.pushBack(u'\u13D1');
        } break;
        case u'\uABA2': {
            queue.pushBack(u'\u13D2');
        } break;
        case u'\uABA3': {
            queue.pushBack(u'\u13D3');
        } break;
        case u'\uABA4': {
            queue.pushBack(u'\u13D4');
        } break;
        case u'\uABA5': {
            queue.pushBack(u'\u13D5');
        } break;
        case u'\uABA6': {
            queue.pushBack(u'\u13D6');
        } break;
        case u'\uABA7': {
            queue.pushBack(u'\u13D7');
        } break;
        case u'\uABA8': {
            queue.pushBack(u'\u13D8');
        } break;
        case u'\uABA9': {
            queue.pushBack(u'\u13D9');
        } break;
        case u'\uABAA': {
            queue.pushBack(u'\u13DA');
        } break;
        case u'\uABAB': {
            queue.pushBack(u'\u13DB');
        } break;
        case u'\uABAC': {
            queue.pushBack(u'\u13DC');
        } break;
        case u'\uABAD': {
            queue.pushBack(u'\u13DD');
        } break;
        case u'\uABAE': {
            queue.pushBack(u'\u13DE');
        } break;
        case u'\uABAF': {
            queue.pushBack(u'\u13DF');
        } break;
        case u'\uABB0': {
            queue.pushBack(u'\u13E0');
        } break;
        case u'\uABB1': {
            queue.pushBack(u'\u13E1');
        } break;
        case u'\uABB2': {
            queue.pushBack(u'\u13E2');
        } break;
        case u'\uABB3': {
            queue.pushBack(u'\u13E3');
        } break;
        case u'\uABB4': {
            queue.pushBack(u'\u13E4');
        } break;
        case u'\uABB5': {
            queue.pushBack(u'\u13E5');
        } break;
        case u'\uABB6': {
            queue.pushBack(u'\u13E6');
        } break;
        case u'\uABB7': {
            queue.pushBack(u'\u13E7');
        } break;
        case u'\uABB8': {
            queue.pushBack(u'\u13E8');
        } break;
        case u'\uABB9': {
            queue.pushBack(u'\u13E9');
        } break;
        case u'\uABBA': {
            queue.pushBack(u'\u13EA');
        } break;
        case u'\uABBB': {
            queue.pushBack(u'\u13EB');
        } break;
        case u'\uABBC': {
            queue.pushBack(u'\u13EC');
        } break;
        case u'\uABBD': {
            queue.pushBack(u'\u13ED');
        } break;
        case u'\uABBE': {
            queue.pushBack(u'\u13EE');
        } break;
        case u'\uABBF': {
            queue.pushBack(u'\u13EF');
        } break;
        case u'\uFB00': {
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u0066');
        } break;
        case u'\uFB01': {
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u0069');
        } break;
        case u'\uFB02': {
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u006C');
        } break;
        case u'\uFB03': {
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u0069');
        } break;
        case u'\uFB04': {
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u0066');
            queue.pushBack(u'\u006C');
        } break;
        case u'\uFB05': [[fallthrough]];
        case u'\uFB06': {
            queue.pushBack(u'\u0073');
            queue.pushBack(u'\u0074');
        } break;
        case u'\uFB13': {
            queue.pushBack(u'\u0574');
            queue.pushBack(u'\u0576');
        } break;
        case u'\uFB14': {
            queue.pushBack(u'\u0574');
            queue.pushBack(u'\u0565');
        } break;
        case u'\uFB15': {
            queue.pushBack(u'\u0574');
            queue.pushBack(u'\u056B');
        } break;
        case u'\uFB16': {
            queue.pushBack(u'\u057E');
            queue.pushBack(u'\u0576');
        } break;
        case u'\uFB17': {
            queue.pushBack(u'\u0574');
            queue.pushBack(u'\u056D');
        } break;
        case u'\uFF21': {
            queue.pushBack(u'\uFF41');
        } break;
        case u'\uFF22': {
            queue.pushBack(u'\uFF42');
        } break;
        case u'\uFF23': {
            queue.pushBack(u'\uFF43');
        } break;
        case u'\uFF24': {
            queue.pushBack(u'\uFF44');
        } break;
        case u'\uFF25': {
            queue.pushBack(u'\uFF45');
        } break;
        case u'\uFF26': {
            queue.pushBack(u'\uFF46');
        } break;
        case u'\uFF27': {
            queue.pushBack(u'\uFF47');
        } break;
        case u'\uFF28': {
            queue.pushBack(u'\uFF48');
        } break;
        case u'\uFF29': {
            queue.pushBack(u'\uFF49');
        } break;
        case u'\uFF2A': {
            queue.pushBack(u'\uFF4A');
        } break;
        case u'\uFF2B': {
            queue.pushBack(u'\uFF4B');
        } break;
        case u'\uFF2C': {
            queue.pushBack(u'\uFF4C');
        } break;
        case u'\uFF2D': {
            queue.pushBack(u'\uFF4D');
        } break;
        case u'\uFF2E': {
            queue.pushBack(u'\uFF4E');
        } break;
        case u'\uFF2F': {
            queue.pushBack(u'\uFF4F');
        } break;
        case u'\uFF30': {
            queue.pushBack(u'\uFF50');
        } break;
        case u'\uFF31': {
            queue.pushBack(u'\uFF51');
        } break;
        case u'\uFF32': {
            queue.pushBack(u'\uFF52');
        } break;
        case u'\uFF33': {
            queue.pushBack(u'\uFF53');
        } break;
        case u'\uFF34': {
            queue.pushBack(u'\uFF54');
        } break;
        case u'\uFF35': {
            queue.pushBack(u'\uFF55');
        } break;
        case u'\uFF36': {
            queue.pushBack(u'\uFF56');
        } break;
        case u'\uFF37': {
            queue.pushBack(u'\uFF57');
        } break;
        case u'\uFF38': {
            queue.pushBack(u'\uFF58');
        } break;
        case u'\uFF39': {
            queue.pushBack(u'\uFF59');
        } break;
        case u'\uFF3A': {
            queue.pushBack(u'\uFF5A');
        } break;
        default: {
            queue.pushBack(ch);
        } break;
    }
}

void Unicode::up(String& str, char16 ch) {
    switch (ch) {
        case u'\u0061': {
            str += u'\u0041';
        } break;
        case u'\u0062': {
            str += u'\u0042';
        } break;
        case u'\u0063': {
            str += u'\u0043';
        } break;
        case u'\u0064': {
            str += u'\u0044';
        } break;
        case u'\u0065': {
            str += u'\u0045';
        } break;
        case u'\u0066': {
            str += u'\u0046';
        } break;
        case u'\u0067': {
            str += u'\u0047';
        } break;
        case u'\u0068': {
            str += u'\u0048';
        } break;
        case u'\u0069': [[fallthrough]];
        case u'\u0131': {
            str += u'\u0049';
        } break;
        case u'\u006A': {
            str += u'\u004A';
        } break;
        case u'\u006B': {
            str += u'\u004B';
        } break;
        case u'\u006C': {
            str += u'\u004C';
        } break;
        case u'\u006D': {
            str += u'\u004D';
        } break;
        case u'\u006E': {
            str += u'\u004E';
        } break;
        case u'\u006F': {
            str += u'\u004F';
        } break;
        case u'\u0070': {
            str += u'\u0050';
        } break;
        case u'\u0071': {
            str += u'\u0051';
        } break;
        case u'\u0072': {
            str += u'\u0052';
        } break;
        case u'\u0073': [[fallthrough]];
        case u'\u017F': {
            str += u'\u0053';
        } break;
        case u'\u0074': {
            str += u'\u0054';
        } break;
        case u'\u0075': {
            str += u'\u0055';
        } break;
        case u'\u0076': {
            str += u'\u0056';
        } break;
        case u'\u0077': {
            str += u'\u0057';
        } break;
        case u'\u0078': {
            str += u'\u0058';
        } break;
        case u'\u0079': {
            str += u'\u0059';
        } break;
        case u'\u007A': {
            str += u'\u005A';
        } break;
        case u'\u00B5': [[fallthrough]];
        case u'\u03BC': {
            str += u'\u039C';
        } break;
        case u'\u00DF': {
            str += u'\u1E9E';
        } break;
        case u'\u00E0': {
            str += u'\u00C0';
        } break;
        case u'\u00E1': {
            str += u'\u00C1';
        } break;
        case u'\u00E2': {
            str += u'\u00C2';
        } break;
        case u'\u00E3': {
            str += u'\u00C3';
        } break;
        case u'\u00E4': {
            str += u'\u00C4';
        } break;
        case u'\u00E5': {
            str += u'\u00C5';
        } break;
        case u'\u00E6': {
            str += u'\u00C6';
        } break;
        case u'\u00E7': {
            str += u'\u00C7';
        } break;
        case u'\u00E8': {
            str += u'\u00C8';
        } break;
        case u'\u00E9': {
            str += u'\u00C9';
        } break;
        case u'\u00EA': {
            str += u'\u00CA';
        } break;
        case u'\u00EB': {
            str += u'\u00CB';
        } break;
        case u'\u00EC': {
            str += u'\u00CC';
        } break;
        case u'\u00ED': {
            str += u'\u00CD';
        } break;
        case u'\u00EE': {
            str += u'\u00CE';
        } break;
        case u'\u00EF': {
            str += u'\u00CF';
        } break;
        case u'\u00F0': {
            str += u'\u00D0';
        } break;
        case u'\u00F1': {
            str += u'\u00D1';
        } break;
        case u'\u00F2': {
            str += u'\u00D2';
        } break;
        case u'\u00F3': {
            str += u'\u00D3';
        } break;
        case u'\u00F4': {
            str += u'\u00D4';
        } break;
        case u'\u00F5': {
            str += u'\u00D5';
        } break;
        case u'\u00F6': {
            str += u'\u00D6';
        } break;
        case u'\u00F8': {
            str += u'\u00D8';
        } break;
        case u'\u00F9': {
            str += u'\u00D9';
        } break;
        case u'\u00FA': {
            str += u'\u00DA';
        } break;
        case u'\u00FB': {
            str += u'\u00DB';
        } break;
        case u'\u00FC': {
            str += u'\u00DC';
        } break;
        case u'\u00FD': {
            str += u'\u00DD';
        } break;
        case u'\u00FE': {
            str += u'\u00DE';
        } break;
        case u'\u00FF': {
            str += u'\u0178';
        } break;
        case u'\u0101': {
            str += u'\u0100';
        } break;
        case u'\u0103': {
            str += u'\u0102';
        } break;
        case u'\u0105': {
            str += u'\u0104';
        } break;
        case u'\u0107': {
            str += u'\u0106';
        } break;
        case u'\u0109': {
            str += u'\u0108';
        } break;
        case u'\u010B': {
            str += u'\u010A';
        } break;
        case u'\u010D': {
            str += u'\u010C';
        } break;
        case u'\u010F': {
            str += u'\u010E';
        } break;
        case u'\u0111': {
            str += u'\u0110';
        } break;
        case u'\u0113': {
            str += u'\u0112';
        } break;
        case u'\u0115': {
            str += u'\u0114';
        } break;
        case u'\u0117': {
            str += u'\u0116';
        } break;
        case u'\u0119': {
            str += u'\u0118';
        } break;
        case u'\u011B': {
            str += u'\u011A';
        } break;
        case u'\u011D': {
            str += u'\u011C';
        } break;
        case u'\u011F': {
            str += u'\u011E';
        } break;
        case u'\u0121': {
            str += u'\u0120';
        } break;
        case u'\u0123': {
            str += u'\u0122';
        } break;
        case u'\u0125': {
            str += u'\u0124';
        } break;
        case u'\u0127': {
            str += u'\u0126';
        } break;
        case u'\u0129': {
            str += u'\u0128';
        } break;
        case u'\u012B': {
            str += u'\u012A';
        } break;
        case u'\u012D': {
            str += u'\u012C';
        } break;
        case u'\u012F': {
            str += u'\u012E';
        } break;
        case u'\u0133': {
            str += u'\u0132';
        } break;
        case u'\u0135': {
            str += u'\u0134';
        } break;
        case u'\u0137': {
            str += u'\u0136';
        } break;
        case u'\u013A': {
            str += u'\u0139';
        } break;
        case u'\u013C': {
            str += u'\u013B';
        } break;
        case u'\u013E': {
            str += u'\u013D';
        } break;
        case u'\u0140': {
            str += u'\u013F';
        } break;
        case u'\u0142': {
            str += u'\u0141';
        } break;
        case u'\u0144': {
            str += u'\u0143';
        } break;
        case u'\u0146': {
            str += u'\u0145';
        } break;
        case u'\u0148': {
            str += u'\u0147';
        } break;
        case u'\u0149': {
            str += u'\u02BC';
            str += u'\u004E';
        } break;
        case u'\u014B': {
            str += u'\u014A';
        } break;
        case u'\u014D': {
            str += u'\u014C';
        } break;
        case u'\u014F': {
            str += u'\u014E';
        } break;
        case u'\u0151': {
            str += u'\u0150';
        } break;
        case u'\u0153': {
            str += u'\u0152';
        } break;
        case u'\u0155': {
            str += u'\u0154';
        } break;
        case u'\u0157': {
            str += u'\u0156';
        } break;
        case u'\u0159': {
            str += u'\u0158';
        } break;
        case u'\u015B': {
            str += u'\u015A';
        } break;
        case u'\u015D': {
            str += u'\u015C';
        } break;
        case u'\u015F': {
            str += u'\u015E';
        } break;
        case u'\u0161': {
            str += u'\u0160';
        } break;
        case u'\u0163': {
            str += u'\u0162';
        } break;
        case u'\u0165': {
            str += u'\u0164';
        } break;
        case u'\u0167': {
            str += u'\u0166';
        } break;
        case u'\u0169': {
            str += u'\u0168';
        } break;
        case u'\u016B': {
            str += u'\u016A';
        } break;
        case u'\u016D': {
            str += u'\u016C';
        } break;
        case u'\u016F': {
            str += u'\u016E';
        } break;
        case u'\u0171': {
            str += u'\u0170';
        } break;
        case u'\u0173': {
            str += u'\u0172';
        } break;
        case u'\u0175': {
            str += u'\u0174';
        } break;
        case u'\u0177': {
            str += u'\u0176';
        } break;
        case u'\u017A': {
            str += u'\u0179';
        } break;
        case u'\u017C': {
            str += u'\u017B';
        } break;
        case u'\u017E': {
            str += u'\u017D';
        } break;
        case u'\u0180': {
            str += u'\u0243';
        } break;
        case u'\u0183': {
            str += u'\u0182';
        } break;
        case u'\u0185': {
            str += u'\u0184';
        } break;
        case u'\u0188': {
            str += u'\u0187';
        } break;
        case u'\u018C': {
            str += u'\u018B';
        } break;
        case u'\u0192': {
            str += u'\u0191';
        } break;
        case u'\u0195': {
            str += u'\u01F6';
        } break;
        case u'\u0199': {
            str += u'\u0198';
        } break;
        case u'\u019A': {
            str += u'\u023D';
        } break;
        case u'\u019E': {
            str += u'\u0220';
        } break;
        case u'\u01A1': {
            str += u'\u01A0';
        } break;
        case u'\u01A3': {
            str += u'\u01A2';
        } break;
        case u'\u01A5': {
            str += u'\u01A4';
        } break;
        case u'\u01A8': {
            str += u'\u01A7';
        } break;
        case u'\u01AD': {
            str += u'\u01AC';
        } break;
        case u'\u01B0': {
            str += u'\u01AF';
        } break;
        case u'\u01B4': {
            str += u'\u01B3';
        } break;
        case u'\u01B6': {
            str += u'\u01B5';
        } break;
        case u'\u01B9': {
            str += u'\u01B8';
        } break;
        case u'\u01BD': {
            str += u'\u01BC';
        } break;
        case u'\u01BF': {
            str += u'\u01F7';
        } break;
        case u'\u01C5': [[fallthrough]];
        case u'\u01C6': {
            str += u'\u01C4';
        } break;
        case u'\u01C8': [[fallthrough]];
        case u'\u01C9': {
            str += u'\u01C7';
        } break;
        case u'\u01CB': [[fallthrough]];
        case u'\u01CC': {
            str += u'\u01CA';
        } break;
        case u'\u01CE': {
            str += u'\u01CD';
        } break;
        case u'\u01D0': {
            str += u'\u01CF';
        } break;
        case u'\u01D2': {
            str += u'\u01D1';
        } break;
        case u'\u01D4': {
            str += u'\u01D3';
        } break;
        case u'\u01D6': {
            str += u'\u01D5';
        } break;
        case u'\u01D8': {
            str += u'\u01D7';
        } break;
        case u'\u01DA': {
            str += u'\u01D9';
        } break;
        case u'\u01DC': {
            str += u'\u01DB';
        } break;
        case u'\u01DD': {
            str += u'\u018E';
        } break;
        case u'\u01DF': {
            str += u'\u01DE';
        } break;
        case u'\u01E1': {
            str += u'\u01E0';
        } break;
        case u'\u01E3': {
            str += u'\u01E2';
        } break;
        case u'\u01E5': {
            str += u'\u01E4';
        } break;
        case u'\u01E7': {
            str += u'\u01E6';
        } break;
        case u'\u01E9': {
            str += u'\u01E8';
        } break;
        case u'\u01EB': {
            str += u'\u01EA';
        } break;
        case u'\u01ED': {
            str += u'\u01EC';
        } break;
        case u'\u01EF': {
            str += u'\u01EE';
        } break;
        case u'\u01F0': {
            str += u'\u004A';
            str += u'\u030C';
        } break;
        case u'\u01F2': [[fallthrough]];
        case u'\u01F3': {
            str += u'\u01F1';
        } break;
        case u'\u01F5': {
            str += u'\u01F4';
        } break;
        case u'\u01F9': {
            str += u'\u01F8';
        } break;
        case u'\u01FB': {
            str += u'\u01FA';
        } break;
        case u'\u01FD': {
            str += u'\u01FC';
        } break;
        case u'\u01FF': {
            str += u'\u01FE';
        } break;
        case u'\u0201': {
            str += u'\u0200';
        } break;
        case u'\u0203': {
            str += u'\u0202';
        } break;
        case u'\u0205': {
            str += u'\u0204';
        } break;
        case u'\u0207': {
            str += u'\u0206';
        } break;
        case u'\u0209': {
            str += u'\u0208';
        } break;
        case u'\u020B': {
            str += u'\u020A';
        } break;
        case u'\u020D': {
            str += u'\u020C';
        } break;
        case u'\u020F': {
            str += u'\u020E';
        } break;
        case u'\u0211': {
            str += u'\u0210';
        } break;
        case u'\u0213': {
            str += u'\u0212';
        } break;
        case u'\u0215': {
            str += u'\u0214';
        } break;
        case u'\u0217': {
            str += u'\u0216';
        } break;
        case u'\u0219': {
            str += u'\u0218';
        } break;
        case u'\u021B': {
            str += u'\u021A';
        } break;
        case u'\u021D': {
            str += u'\u021C';
        } break;
        case u'\u021F': {
            str += u'\u021E';
        } break;
        case u'\u0223': {
            str += u'\u0222';
        } break;
        case u'\u0225': {
            str += u'\u0224';
        } break;
        case u'\u0227': {
            str += u'\u0226';
        } break;
        case u'\u0229': {
            str += u'\u0228';
        } break;
        case u'\u022B': {
            str += u'\u022A';
        } break;
        case u'\u022D': {
            str += u'\u022C';
        } break;
        case u'\u022F': {
            str += u'\u022E';
        } break;
        case u'\u0231': {
            str += u'\u0230';
        } break;
        case u'\u0233': {
            str += u'\u0232';
        } break;
        case u'\u023C': {
            str += u'\u023B';
        } break;
        case u'\u023F': {
            str += u'\u2C7E';
        } break;
        case u'\u0240': {
            str += u'\u2C7F';
        } break;
        case u'\u0242': {
            str += u'\u0241';
        } break;
        case u'\u0247': {
            str += u'\u0246';
        } break;
        case u'\u0249': {
            str += u'\u0248';
        } break;
        case u'\u024B': {
            str += u'\u024A';
        } break;
        case u'\u024D': {
            str += u'\u024C';
        } break;
        case u'\u024F': {
            str += u'\u024E';
        } break;
        case u'\u0250': {
            str += u'\u2C6F';
        } break;
        case u'\u0251': {
            str += u'\u2C6D';
        } break;
        case u'\u0252': {
            str += u'\u2C70';
        } break;
        case u'\u0253': {
            str += u'\u0181';
        } break;
        case u'\u0254': {
            str += u'\u0186';
        } break;
        case u'\u0256': {
            str += u'\u0189';
        } break;
        case u'\u0257': {
            str += u'\u018A';
        } break;
        case u'\u0259': {
            str += u'\u018F';
        } break;
        case u'\u025B': {
            str += u'\u0190';
        } break;
        case u'\u025C': {
            str += u'\uA7AB';
        } break;
        case u'\u0260': {
            str += u'\u0193';
        } break;
        case u'\u0261': {
            str += u'\uA7AC';
        } break;
        case u'\u0263': {
            str += u'\u0194';
        } break;
        case u'\u0265': {
            str += u'\uA78D';
        } break;
        case u'\u0266': {
            str += u'\uA7AA';
        } break;
        case u'\u0268': {
            str += u'\u0197';
        } break;
        case u'\u0269': {
            str += u'\u0196';
        } break;
        case u'\u026A': {
            str += u'\uA7AE';
        } break;
        case u'\u026B': {
            str += u'\u2C62';
        } break;
        case u'\u026C': {
            str += u'\uA7AD';
        } break;
        case u'\u026F': {
            str += u'\u019C';
        } break;
        case u'\u0271': {
            str += u'\u2C6E';
        } break;
        case u'\u0272': {
            str += u'\u019D';
        } break;
        case u'\u0275': {
            str += u'\u019F';
        } break;
        case u'\u027D': {
            str += u'\u2C64';
        } break;
        case u'\u0280': {
            str += u'\u01A6';
        } break;
        case u'\u0282': {
            str += u'\uA7C5';
        } break;
        case u'\u0283': {
            str += u'\u01A9';
        } break;
        case u'\u0287': {
            str += u'\uA7B1';
        } break;
        case u'\u0288': {
            str += u'\u01AE';
        } break;
        case u'\u0289': {
            str += u'\u0244';
        } break;
        case u'\u028A': {
            str += u'\u01B1';
        } break;
        case u'\u028B': {
            str += u'\u01B2';
        } break;
        case u'\u028C': {
            str += u'\u0245';
        } break;
        case u'\u0292': {
            str += u'\u01B7';
        } break;
        case u'\u029D': {
            str += u'\uA7B2';
        } break;
        case u'\u029E': {
            str += u'\uA7B0';
        } break;
        case u'\u0345': [[fallthrough]];
        case u'\u03B9': [[fallthrough]];
        case u'\u1FBE': {
            str += u'\u0399';
        } break;
        case u'\u0371': {
            str += u'\u0370';
        } break;
        case u'\u0373': {
            str += u'\u0372';
        } break;
        case u'\u0377': {
            str += u'\u0376';
        } break;
        case u'\u037B': {
            str += u'\u03FD';
        } break;
        case u'\u037C': {
            str += u'\u03FE';
        } break;
        case u'\u037D': {
            str += u'\u03FF';
        } break;
        case u'\u0390': [[fallthrough]];
        case u'\u1FD3': {
            str += u'\u0399';
            str += u'\u0308';
            str += u'\u0301';
        } break;
        case u'\u03AC': {
            str += u'\u0386';
        } break;
        case u'\u03AD': {
            str += u'\u0388';
        } break;
        case u'\u03AE': {
            str += u'\u0389';
        } break;
        case u'\u03AF': {
            str += u'\u038A';
        } break;
        case u'\u03B0': [[fallthrough]];
        case u'\u1FE3': {
            str += u'\u03A5';
            str += u'\u0308';
            str += u'\u0301';
        } break;
        case u'\u03B1': {
            str += u'\u0391';
        } break;
        case u'\u03B2': [[fallthrough]];
        case u'\u03D0': {
            str += u'\u0392';
        } break;
        case u'\u03B3': {
            str += u'\u0393';
        } break;
        case u'\u03B4': {
            str += u'\u0394';
        } break;
        case u'\u03B5': [[fallthrough]];
        case u'\u03F5': {
            str += u'\u0395';
        } break;
        case u'\u03B6': {
            str += u'\u0396';
        } break;
        case u'\u03B7': {
            str += u'\u0397';
        } break;
        case u'\u03B8': [[fallthrough]];
        case u'\u03D1': {
            str += u'\u0398';
        } break;
        case u'\u03BA': [[fallthrough]];
        case u'\u03F0': {
            str += u'\u039A';
        } break;
        case u'\u03BB': {
            str += u'\u039B';
        } break;
        case u'\u03BD': {
            str += u'\u039D';
        } break;
        case u'\u03BE': {
            str += u'\u039E';
        } break;
        case u'\u03BF': {
            str += u'\u039F';
        } break;
        case u'\u03C0': [[fallthrough]];
        case u'\u03D6': {
            str += u'\u03A0';
        } break;
        case u'\u03C1': [[fallthrough]];
        case u'\u03F1': {
            str += u'\u03A1';
        } break;
        case u'\u03C2': [[fallthrough]];
        case u'\u03C3': {
            str += u'\u03A3';
        } break;
        case u'\u03C4': {
            str += u'\u03A4';
        } break;
        case u'\u03C5': {
            str += u'\u03A5';
        } break;
        case u'\u03C6': [[fallthrough]];
        case u'\u03D5': {
            str += u'\u03A6';
        } break;
        case u'\u03C7': {
            str += u'\u03A7';
        } break;
        case u'\u03C8': {
            str += u'\u03A8';
        } break;
        case u'\u03C9': {
            str += u'\u03A9';
        } break;
        case u'\u03CA': {
            str += u'\u03AA';
        } break;
        case u'\u03CB': {
            str += u'\u03AB';
        } break;
        case u'\u03CC': {
            str += u'\u038C';
        } break;
        case u'\u03CD': {
            str += u'\u038E';
        } break;
        case u'\u03CE': {
            str += u'\u038F';
        } break;
        case u'\u03D7': {
            str += u'\u03CF';
        } break;
        case u'\u03D9': {
            str += u'\u03D8';
        } break;
        case u'\u03DB': {
            str += u'\u03DA';
        } break;
        case u'\u03DD': {
            str += u'\u03DC';
        } break;
        case u'\u03DF': {
            str += u'\u03DE';
        } break;
        case u'\u03E1': {
            str += u'\u03E0';
        } break;
        case u'\u03E3': {
            str += u'\u03E2';
        } break;
        case u'\u03E5': {
            str += u'\u03E4';
        } break;
        case u'\u03E7': {
            str += u'\u03E6';
        } break;
        case u'\u03E9': {
            str += u'\u03E8';
        } break;
        case u'\u03EB': {
            str += u'\u03EA';
        } break;
        case u'\u03ED': {
            str += u'\u03EC';
        } break;
        case u'\u03EF': {
            str += u'\u03EE';
        } break;
        case u'\u03F2': {
            str += u'\u03F9';
        } break;
        case u'\u03F3': {
            str += u'\u037F';
        } break;
        case u'\u03F8': {
            str += u'\u03F7';
        } break;
        case u'\u03FB': {
            str += u'\u03FA';
        } break;
        case u'\u0430': {
            str += u'\u0410';
        } break;
        case u'\u0431': {
            str += u'\u0411';
        } break;
        case u'\u0432': [[fallthrough]];
        case u'\u1C80': {
            str += u'\u0412';
        } break;
        case u'\u0433': {
            str += u'\u0413';
        } break;
        case u'\u0434': [[fallthrough]];
        case u'\u1C81': {
            str += u'\u0414';
        } break;
        case u'\u0435': {
            str += u'\u0415';
        } break;
        case u'\u0436': {
            str += u'\u0416';
        } break;
        case u'\u0437': {
            str += u'\u0417';
        } break;
        case u'\u0438': {
            str += u'\u0418';
        } break;
        case u'\u0439': {
            str += u'\u0419';
        } break;
        case u'\u043A': {
            str += u'\u041A';
        } break;
        case u'\u043B': {
            str += u'\u041B';
        } break;
        case u'\u043C': {
            str += u'\u041C';
        } break;
        case u'\u043D': {
            str += u'\u041D';
        } break;
        case u'\u043E': [[fallthrough]];
        case u'\u1C82': {
            str += u'\u041E';
        } break;
        case u'\u043F': {
            str += u'\u041F';
        } break;
        case u'\u0440': {
            str += u'\u0420';
        } break;
        case u'\u0441': [[fallthrough]];
        case u'\u1C83': {
            str += u'\u0421';
        } break;
        case u'\u0442': [[fallthrough]];
        case u'\u1C84': [[fallthrough]];
        case u'\u1C85': {
            str += u'\u0422';
        } break;
        case u'\u0443': {
            str += u'\u0423';
        } break;
        case u'\u0444': {
            str += u'\u0424';
        } break;
        case u'\u0445': {
            str += u'\u0425';
        } break;
        case u'\u0446': {
            str += u'\u0426';
        } break;
        case u'\u0447': {
            str += u'\u0427';
        } break;
        case u'\u0448': {
            str += u'\u0428';
        } break;
        case u'\u0449': {
            str += u'\u0429';
        } break;
        case u'\u044A': [[fallthrough]];
        case u'\u1C86': {
            str += u'\u042A';
        } break;
        case u'\u044B': {
            str += u'\u042B';
        } break;
        case u'\u044C': {
            str += u'\u042C';
        } break;
        case u'\u044D': {
            str += u'\u042D';
        } break;
        case u'\u044E': {
            str += u'\u042E';
        } break;
        case u'\u044F': {
            str += u'\u042F';
        } break;
        case u'\u0450': {
            str += u'\u0400';
        } break;
        case u'\u0451': {
            str += u'\u0401';
        } break;
        case u'\u0452': {
            str += u'\u0402';
        } break;
        case u'\u0453': {
            str += u'\u0403';
        } break;
        case u'\u0454': {
            str += u'\u0404';
        } break;
        case u'\u0455': {
            str += u'\u0405';
        } break;
        case u'\u0456': {
            str += u'\u0406';
        } break;
        case u'\u0457': {
            str += u'\u0407';
        } break;
        case u'\u0458': {
            str += u'\u0408';
        } break;
        case u'\u0459': {
            str += u'\u0409';
        } break;
        case u'\u045A': {
            str += u'\u040A';
        } break;
        case u'\u045B': {
            str += u'\u040B';
        } break;
        case u'\u045C': {
            str += u'\u040C';
        } break;
        case u'\u045D': {
            str += u'\u040D';
        } break;
        case u'\u045E': {
            str += u'\u040E';
        } break;
        case u'\u045F': {
            str += u'\u040F';
        } break;
        case u'\u0461': {
            str += u'\u0460';
        } break;
        case u'\u0463': [[fallthrough]];
        case u'\u1C87': {
            str += u'\u0462';
        } break;
        case u'\u0465': {
            str += u'\u0464';
        } break;
        case u'\u0467': {
            str += u'\u0466';
        } break;
        case u'\u0469': {
            str += u'\u0468';
        } break;
        case u'\u046B': {
            str += u'\u046A';
        } break;
        case u'\u046D': {
            str += u'\u046C';
        } break;
        case u'\u046F': {
            str += u'\u046E';
        } break;
        case u'\u0471': {
            str += u'\u0470';
        } break;
        case u'\u0473': {
            str += u'\u0472';
        } break;
        case u'\u0475': {
            str += u'\u0474';
        } break;
        case u'\u0477': {
            str += u'\u0476';
        } break;
        case u'\u0479': {
            str += u'\u0478';
        } break;
        case u'\u047B': {
            str += u'\u047A';
        } break;
        case u'\u047D': {
            str += u'\u047C';
        } break;
        case u'\u047F': {
            str += u'\u047E';
        } break;
        case u'\u0481': {
            str += u'\u0480';
        } break;
        case u'\u048B': {
            str += u'\u048A';
        } break;
        case u'\u048D': {
            str += u'\u048C';
        } break;
        case u'\u048F': {
            str += u'\u048E';
        } break;
        case u'\u0491': {
            str += u'\u0490';
        } break;
        case u'\u0493': {
            str += u'\u0492';
        } break;
        case u'\u0495': {
            str += u'\u0494';
        } break;
        case u'\u0497': {
            str += u'\u0496';
        } break;
        case u'\u0499': {
            str += u'\u0498';
        } break;
        case u'\u049B': {
            str += u'\u049A';
        } break;
        case u'\u049D': {
            str += u'\u049C';
        } break;
        case u'\u049F': {
            str += u'\u049E';
        } break;
        case u'\u04A1': {
            str += u'\u04A0';
        } break;
        case u'\u04A3': {
            str += u'\u04A2';
        } break;
        case u'\u04A5': {
            str += u'\u04A4';
        } break;
        case u'\u04A7': {
            str += u'\u04A6';
        } break;
        case u'\u04A9': {
            str += u'\u04A8';
        } break;
        case u'\u04AB': {
            str += u'\u04AA';
        } break;
        case u'\u04AD': {
            str += u'\u04AC';
        } break;
        case u'\u04AF': {
            str += u'\u04AE';
        } break;
        case u'\u04B1': {
            str += u'\u04B0';
        } break;
        case u'\u04B3': {
            str += u'\u04B2';
        } break;
        case u'\u04B5': {
            str += u'\u04B4';
        } break;
        case u'\u04B7': {
            str += u'\u04B6';
        } break;
        case u'\u04B9': {
            str += u'\u04B8';
        } break;
        case u'\u04BB': {
            str += u'\u04BA';
        } break;
        case u'\u04BD': {
            str += u'\u04BC';
        } break;
        case u'\u04BF': {
            str += u'\u04BE';
        } break;
        case u'\u04C2': {
            str += u'\u04C1';
        } break;
        case u'\u04C4': {
            str += u'\u04C3';
        } break;
        case u'\u04C6': {
            str += u'\u04C5';
        } break;
        case u'\u04C8': {
            str += u'\u04C7';
        } break;
        case u'\u04CA': {
            str += u'\u04C9';
        } break;
        case u'\u04CC': {
            str += u'\u04CB';
        } break;
        case u'\u04CE': {
            str += u'\u04CD';
        } break;
        case u'\u04CF': {
            str += u'\u04C0';
        } break;
        case u'\u04D1': {
            str += u'\u04D0';
        } break;
        case u'\u04D3': {
            str += u'\u04D2';
        } break;
        case u'\u04D5': {
            str += u'\u04D4';
        } break;
        case u'\u04D7': {
            str += u'\u04D6';
        } break;
        case u'\u04D9': {
            str += u'\u04D8';
        } break;
        case u'\u04DB': {
            str += u'\u04DA';
        } break;
        case u'\u04DD': {
            str += u'\u04DC';
        } break;
        case u'\u04DF': {
            str += u'\u04DE';
        } break;
        case u'\u04E1': {
            str += u'\u04E0';
        } break;
        case u'\u04E3': {
            str += u'\u04E2';
        } break;
        case u'\u04E5': {
            str += u'\u04E4';
        } break;
        case u'\u04E7': {
            str += u'\u04E6';
        } break;
        case u'\u04E9': {
            str += u'\u04E8';
        } break;
        case u'\u04EB': {
            str += u'\u04EA';
        } break;
        case u'\u04ED': {
            str += u'\u04EC';
        } break;
        case u'\u04EF': {
            str += u'\u04EE';
        } break;
        case u'\u04F1': {
            str += u'\u04F0';
        } break;
        case u'\u04F3': {
            str += u'\u04F2';
        } break;
        case u'\u04F5': {
            str += u'\u04F4';
        } break;
        case u'\u04F7': {
            str += u'\u04F6';
        } break;
        case u'\u04F9': {
            str += u'\u04F8';
        } break;
        case u'\u04FB': {
            str += u'\u04FA';
        } break;
        case u'\u04FD': {
            str += u'\u04FC';
        } break;
        case u'\u04FF': {
            str += u'\u04FE';
        } break;
        case u'\u0501': {
            str += u'\u0500';
        } break;
        case u'\u0503': {
            str += u'\u0502';
        } break;
        case u'\u0505': {
            str += u'\u0504';
        } break;
        case u'\u0507': {
            str += u'\u0506';
        } break;
        case u'\u0509': {
            str += u'\u0508';
        } break;
        case u'\u050B': {
            str += u'\u050A';
        } break;
        case u'\u050D': {
            str += u'\u050C';
        } break;
        case u'\u050F': {
            str += u'\u050E';
        } break;
        case u'\u0511': {
            str += u'\u0510';
        } break;
        case u'\u0513': {
            str += u'\u0512';
        } break;
        case u'\u0515': {
            str += u'\u0514';
        } break;
        case u'\u0517': {
            str += u'\u0516';
        } break;
        case u'\u0519': {
            str += u'\u0518';
        } break;
        case u'\u051B': {
            str += u'\u051A';
        } break;
        case u'\u051D': {
            str += u'\u051C';
        } break;
        case u'\u051F': {
            str += u'\u051E';
        } break;
        case u'\u0521': {
            str += u'\u0520';
        } break;
        case u'\u0523': {
            str += u'\u0522';
        } break;
        case u'\u0525': {
            str += u'\u0524';
        } break;
        case u'\u0527': {
            str += u'\u0526';
        } break;
        case u'\u0529': {
            str += u'\u0528';
        } break;
        case u'\u052B': {
            str += u'\u052A';
        } break;
        case u'\u052D': {
            str += u'\u052C';
        } break;
        case u'\u052F': {
            str += u'\u052E';
        } break;
        case u'\u0561': {
            str += u'\u0531';
        } break;
        case u'\u0562': {
            str += u'\u0532';
        } break;
        case u'\u0563': {
            str += u'\u0533';
        } break;
        case u'\u0564': {
            str += u'\u0534';
        } break;
        case u'\u0565': {
            str += u'\u0535';
        } break;
        case u'\u0566': {
            str += u'\u0536';
        } break;
        case u'\u0567': {
            str += u'\u0537';
        } break;
        case u'\u0568': {
            str += u'\u0538';
        } break;
        case u'\u0569': {
            str += u'\u0539';
        } break;
        case u'\u056A': {
            str += u'\u053A';
        } break;
        case u'\u056B': {
            str += u'\u053B';
        } break;
        case u'\u056C': {
            str += u'\u053C';
        } break;
        case u'\u056D': {
            str += u'\u053D';
        } break;
        case u'\u056E': {
            str += u'\u053E';
        } break;
        case u'\u056F': {
            str += u'\u053F';
        } break;
        case u'\u0570': {
            str += u'\u0540';
        } break;
        case u'\u0571': {
            str += u'\u0541';
        } break;
        case u'\u0572': {
            str += u'\u0542';
        } break;
        case u'\u0573': {
            str += u'\u0543';
        } break;
        case u'\u0574': {
            str += u'\u0544';
        } break;
        case u'\u0575': {
            str += u'\u0545';
        } break;
        case u'\u0576': {
            str += u'\u0546';
        } break;
        case u'\u0577': {
            str += u'\u0547';
        } break;
        case u'\u0578': {
            str += u'\u0548';
        } break;
        case u'\u0579': {
            str += u'\u0549';
        } break;
        case u'\u057A': {
            str += u'\u054A';
        } break;
        case u'\u057B': {
            str += u'\u054B';
        } break;
        case u'\u057C': {
            str += u'\u054C';
        } break;
        case u'\u057D': {
            str += u'\u054D';
        } break;
        case u'\u057E': {
            str += u'\u054E';
        } break;
        case u'\u057F': {
            str += u'\u054F';
        } break;
        case u'\u0580': {
            str += u'\u0550';
        } break;
        case u'\u0581': {
            str += u'\u0551';
        } break;
        case u'\u0582': {
            str += u'\u0552';
        } break;
        case u'\u0583': {
            str += u'\u0553';
        } break;
        case u'\u0584': {
            str += u'\u0554';
        } break;
        case u'\u0585': {
            str += u'\u0555';
        } break;
        case u'\u0586': {
            str += u'\u0556';
        } break;
        case u'\u0587': {
            str += u'\u0535';
            str += u'\u0552';
        } break;
        case u'\u10D0': {
            str += u'\u1C90';
        } break;
        case u'\u10D1': {
            str += u'\u1C91';
        } break;
        case u'\u10D2': {
            str += u'\u1C92';
        } break;
        case u'\u10D3': {
            str += u'\u1C93';
        } break;
        case u'\u10D4': {
            str += u'\u1C94';
        } break;
        case u'\u10D5': {
            str += u'\u1C95';
        } break;
        case u'\u10D6': {
            str += u'\u1C96';
        } break;
        case u'\u10D7': {
            str += u'\u1C97';
        } break;
        case u'\u10D8': {
            str += u'\u1C98';
        } break;
        case u'\u10D9': {
            str += u'\u1C99';
        } break;
        case u'\u10DA': {
            str += u'\u1C9A';
        } break;
        case u'\u10DB': {
            str += u'\u1C9B';
        } break;
        case u'\u10DC': {
            str += u'\u1C9C';
        } break;
        case u'\u10DD': {
            str += u'\u1C9D';
        } break;
        case u'\u10DE': {
            str += u'\u1C9E';
        } break;
        case u'\u10DF': {
            str += u'\u1C9F';
        } break;
        case u'\u10E0': {
            str += u'\u1CA0';
        } break;
        case u'\u10E1': {
            str += u'\u1CA1';
        } break;
        case u'\u10E2': {
            str += u'\u1CA2';
        } break;
        case u'\u10E3': {
            str += u'\u1CA3';
        } break;
        case u'\u10E4': {
            str += u'\u1CA4';
        } break;
        case u'\u10E5': {
            str += u'\u1CA5';
        } break;
        case u'\u10E6': {
            str += u'\u1CA6';
        } break;
        case u'\u10E7': {
            str += u'\u1CA7';
        } break;
        case u'\u10E8': {
            str += u'\u1CA8';
        } break;
        case u'\u10E9': {
            str += u'\u1CA9';
        } break;
        case u'\u10EA': {
            str += u'\u1CAA';
        } break;
        case u'\u10EB': {
            str += u'\u1CAB';
        } break;
        case u'\u10EC': {
            str += u'\u1CAC';
        } break;
        case u'\u10ED': {
            str += u'\u1CAD';
        } break;
        case u'\u10EE': {
            str += u'\u1CAE';
        } break;
        case u'\u10EF': {
            str += u'\u1CAF';
        } break;
        case u'\u10F0': {
            str += u'\u1CB0';
        } break;
        case u'\u10F1': {
            str += u'\u1CB1';
        } break;
        case u'\u10F2': {
            str += u'\u1CB2';
        } break;
        case u'\u10F3': {
            str += u'\u1CB3';
        } break;
        case u'\u10F4': {
            str += u'\u1CB4';
        } break;
        case u'\u10F5': {
            str += u'\u1CB5';
        } break;
        case u'\u10F6': {
            str += u'\u1CB6';
        } break;
        case u'\u10F7': {
            str += u'\u1CB7';
        } break;
        case u'\u10F8': {
            str += u'\u1CB8';
        } break;
        case u'\u10F9': {
            str += u'\u1CB9';
        } break;
        case u'\u10FA': {
            str += u'\u1CBA';
        } break;
        case u'\u10FD': {
            str += u'\u1CBD';
        } break;
        case u'\u10FE': {
            str += u'\u1CBE';
        } break;
        case u'\u10FF': {
            str += u'\u1CBF';
        } break;
        case u'\u13F8': {
            str += u'\u13F0';
        } break;
        case u'\u13F9': {
            str += u'\u13F1';
        } break;
        case u'\u13FA': {
            str += u'\u13F2';
        } break;
        case u'\u13FB': {
            str += u'\u13F3';
        } break;
        case u'\u13FC': {
            str += u'\u13F4';
        } break;
        case u'\u13FD': {
            str += u'\u13F5';
        } break;
        case u'\u1C88': [[fallthrough]];
        case u'\uA64B': {
            str += u'\uA64A';
        } break;
        case u'\u1D79': {
            str += u'\uA77D';
        } break;
        case u'\u1D7D': {
            str += u'\u2C63';
        } break;
        case u'\u1D8E': {
            str += u'\uA7C6';
        } break;
        case u'\u1E01': {
            str += u'\u1E00';
        } break;
        case u'\u1E03': {
            str += u'\u1E02';
        } break;
        case u'\u1E05': {
            str += u'\u1E04';
        } break;
        case u'\u1E07': {
            str += u'\u1E06';
        } break;
        case u'\u1E09': {
            str += u'\u1E08';
        } break;
        case u'\u1E0B': {
            str += u'\u1E0A';
        } break;
        case u'\u1E0D': {
            str += u'\u1E0C';
        } break;
        case u'\u1E0F': {
            str += u'\u1E0E';
        } break;
        case u'\u1E11': {
            str += u'\u1E10';
        } break;
        case u'\u1E13': {
            str += u'\u1E12';
        } break;
        case u'\u1E15': {
            str += u'\u1E14';
        } break;
        case u'\u1E17': {
            str += u'\u1E16';
        } break;
        case u'\u1E19': {
            str += u'\u1E18';
        } break;
        case u'\u1E1B': {
            str += u'\u1E1A';
        } break;
        case u'\u1E1D': {
            str += u'\u1E1C';
        } break;
        case u'\u1E1F': {
            str += u'\u1E1E';
        } break;
        case u'\u1E21': {
            str += u'\u1E20';
        } break;
        case u'\u1E23': {
            str += u'\u1E22';
        } break;
        case u'\u1E25': {
            str += u'\u1E24';
        } break;
        case u'\u1E27': {
            str += u'\u1E26';
        } break;
        case u'\u1E29': {
            str += u'\u1E28';
        } break;
        case u'\u1E2B': {
            str += u'\u1E2A';
        } break;
        case u'\u1E2D': {
            str += u'\u1E2C';
        } break;
        case u'\u1E2F': {
            str += u'\u1E2E';
        } break;
        case u'\u1E31': {
            str += u'\u1E30';
        } break;
        case u'\u1E33': {
            str += u'\u1E32';
        } break;
        case u'\u1E35': {
            str += u'\u1E34';
        } break;
        case u'\u1E37': {
            str += u'\u1E36';
        } break;
        case u'\u1E39': {
            str += u'\u1E38';
        } break;
        case u'\u1E3B': {
            str += u'\u1E3A';
        } break;
        case u'\u1E3D': {
            str += u'\u1E3C';
        } break;
        case u'\u1E3F': {
            str += u'\u1E3E';
        } break;
        case u'\u1E41': {
            str += u'\u1E40';
        } break;
        case u'\u1E43': {
            str += u'\u1E42';
        } break;
        case u'\u1E45': {
            str += u'\u1E44';
        } break;
        case u'\u1E47': {
            str += u'\u1E46';
        } break;
        case u'\u1E49': {
            str += u'\u1E48';
        } break;
        case u'\u1E4B': {
            str += u'\u1E4A';
        } break;
        case u'\u1E4D': {
            str += u'\u1E4C';
        } break;
        case u'\u1E4F': {
            str += u'\u1E4E';
        } break;
        case u'\u1E51': {
            str += u'\u1E50';
        } break;
        case u'\u1E53': {
            str += u'\u1E52';
        } break;
        case u'\u1E55': {
            str += u'\u1E54';
        } break;
        case u'\u1E57': {
            str += u'\u1E56';
        } break;
        case u'\u1E59': {
            str += u'\u1E58';
        } break;
        case u'\u1E5B': {
            str += u'\u1E5A';
        } break;
        case u'\u1E5D': {
            str += u'\u1E5C';
        } break;
        case u'\u1E5F': {
            str += u'\u1E5E';
        } break;
        case u'\u1E61': [[fallthrough]];
        case u'\u1E9B': {
            str += u'\u1E60';
        } break;
        case u'\u1E63': {
            str += u'\u1E62';
        } break;
        case u'\u1E65': {
            str += u'\u1E64';
        } break;
        case u'\u1E67': {
            str += u'\u1E66';
        } break;
        case u'\u1E69': {
            str += u'\u1E68';
        } break;
        case u'\u1E6B': {
            str += u'\u1E6A';
        } break;
        case u'\u1E6D': {
            str += u'\u1E6C';
        } break;
        case u'\u1E6F': {
            str += u'\u1E6E';
        } break;
        case u'\u1E71': {
            str += u'\u1E70';
        } break;
        case u'\u1E73': {
            str += u'\u1E72';
        } break;
        case u'\u1E75': {
            str += u'\u1E74';
        } break;
        case u'\u1E77': {
            str += u'\u1E76';
        } break;
        case u'\u1E79': {
            str += u'\u1E78';
        } break;
        case u'\u1E7B': {
            str += u'\u1E7A';
        } break;
        case u'\u1E7D': {
            str += u'\u1E7C';
        } break;
        case u'\u1E7F': {
            str += u'\u1E7E';
        } break;
        case u'\u1E81': {
            str += u'\u1E80';
        } break;
        case u'\u1E83': {
            str += u'\u1E82';
        } break;
        case u'\u1E85': {
            str += u'\u1E84';
        } break;
        case u'\u1E87': {
            str += u'\u1E86';
        } break;
        case u'\u1E89': {
            str += u'\u1E88';
        } break;
        case u'\u1E8B': {
            str += u'\u1E8A';
        } break;
        case u'\u1E8D': {
            str += u'\u1E8C';
        } break;
        case u'\u1E8F': {
            str += u'\u1E8E';
        } break;
        case u'\u1E91': {
            str += u'\u1E90';
        } break;
        case u'\u1E93': {
            str += u'\u1E92';
        } break;
        case u'\u1E95': {
            str += u'\u1E94';
        } break;
        case u'\u1E96': {
            str += u'\u0048';
            str += u'\u0331';
        } break;
        case u'\u1E97': {
            str += u'\u0054';
            str += u'\u0308';
        } break;
        case u'\u1E98': {
            str += u'\u0057';
            str += u'\u030A';
        } break;
        case u'\u1E99': {
            str += u'\u0059';
            str += u'\u030A';
        } break;
        case u'\u1E9A': {
            str += u'\u0041';
            str += u'\u02BE';
        } break;
        case u'\u1EA1': {
            str += u'\u1EA0';
        } break;
        case u'\u1EA3': {
            str += u'\u1EA2';
        } break;
        case u'\u1EA5': {
            str += u'\u1EA4';
        } break;
        case u'\u1EA7': {
            str += u'\u1EA6';
        } break;
        case u'\u1EA9': {
            str += u'\u1EA8';
        } break;
        case u'\u1EAB': {
            str += u'\u1EAA';
        } break;
        case u'\u1EAD': {
            str += u'\u1EAC';
        } break;
        case u'\u1EAF': {
            str += u'\u1EAE';
        } break;
        case u'\u1EB1': {
            str += u'\u1EB0';
        } break;
        case u'\u1EB3': {
            str += u'\u1EB2';
        } break;
        case u'\u1EB5': {
            str += u'\u1EB4';
        } break;
        case u'\u1EB7': {
            str += u'\u1EB6';
        } break;
        case u'\u1EB9': {
            str += u'\u1EB8';
        } break;
        case u'\u1EBB': {
            str += u'\u1EBA';
        } break;
        case u'\u1EBD': {
            str += u'\u1EBC';
        } break;
        case u'\u1EBF': {
            str += u'\u1EBE';
        } break;
        case u'\u1EC1': {
            str += u'\u1EC0';
        } break;
        case u'\u1EC3': {
            str += u'\u1EC2';
        } break;
        case u'\u1EC5': {
            str += u'\u1EC4';
        } break;
        case u'\u1EC7': {
            str += u'\u1EC6';
        } break;
        case u'\u1EC9': {
            str += u'\u1EC8';
        } break;
        case u'\u1ECB': {
            str += u'\u1ECA';
        } break;
        case u'\u1ECD': {
            str += u'\u1ECC';
        } break;
        case u'\u1ECF': {
            str += u'\u1ECE';
        } break;
        case u'\u1ED1': {
            str += u'\u1ED0';
        } break;
        case u'\u1ED3': {
            str += u'\u1ED2';
        } break;
        case u'\u1ED5': {
            str += u'\u1ED4';
        } break;
        case u'\u1ED7': {
            str += u'\u1ED6';
        } break;
        case u'\u1ED9': {
            str += u'\u1ED8';
        } break;
        case u'\u1EDB': {
            str += u'\u1EDA';
        } break;
        case u'\u1EDD': {
            str += u'\u1EDC';
        } break;
        case u'\u1EDF': {
            str += u'\u1EDE';
        } break;
        case u'\u1EE1': {
            str += u'\u1EE0';
        } break;
        case u'\u1EE3': {
            str += u'\u1EE2';
        } break;
        case u'\u1EE5': {
            str += u'\u1EE4';
        } break;
        case u'\u1EE7': {
            str += u'\u1EE6';
        } break;
        case u'\u1EE9': {
            str += u'\u1EE8';
        } break;
        case u'\u1EEB': {
            str += u'\u1EEA';
        } break;
        case u'\u1EED': {
            str += u'\u1EEC';
        } break;
        case u'\u1EEF': {
            str += u'\u1EEE';
        } break;
        case u'\u1EF1': {
            str += u'\u1EF0';
        } break;
        case u'\u1EF3': {
            str += u'\u1EF2';
        } break;
        case u'\u1EF5': {
            str += u'\u1EF4';
        } break;
        case u'\u1EF7': {
            str += u'\u1EF6';
        } break;
        case u'\u1EF9': {
            str += u'\u1EF8';
        } break;
        case u'\u1EFB': {
            str += u'\u1EFA';
        } break;
        case u'\u1EFD': {
            str += u'\u1EFC';
        } break;
        case u'\u1EFF': {
            str += u'\u1EFE';
        } break;
        case u'\u1F00': {
            str += u'\u1F08';
        } break;
        case u'\u1F01': {
            str += u'\u1F09';
        } break;
        case u'\u1F02': {
            str += u'\u1F0A';
        } break;
        case u'\u1F03': {
            str += u'\u1F0B';
        } break;
        case u'\u1F04': {
            str += u'\u1F0C';
        } break;
        case u'\u1F05': {
            str += u'\u1F0D';
        } break;
        case u'\u1F06': {
            str += u'\u1F0E';
        } break;
        case u'\u1F07': {
            str += u'\u1F0F';
        } break;
        case u'\u1F10': {
            str += u'\u1F18';
        } break;
        case u'\u1F11': {
            str += u'\u1F19';
        } break;
        case u'\u1F12': {
            str += u'\u1F1A';
        } break;
        case u'\u1F13': {
            str += u'\u1F1B';
        } break;
        case u'\u1F14': {
            str += u'\u1F1C';
        } break;
        case u'\u1F15': {
            str += u'\u1F1D';
        } break;
        case u'\u1F20': {
            str += u'\u1F28';
        } break;
        case u'\u1F21': {
            str += u'\u1F29';
        } break;
        case u'\u1F22': {
            str += u'\u1F2A';
        } break;
        case u'\u1F23': {
            str += u'\u1F2B';
        } break;
        case u'\u1F24': {
            str += u'\u1F2C';
        } break;
        case u'\u1F25': {
            str += u'\u1F2D';
        } break;
        case u'\u1F26': {
            str += u'\u1F2E';
        } break;
        case u'\u1F27': {
            str += u'\u1F2F';
        } break;
        case u'\u1F30': {
            str += u'\u1F38';
        } break;
        case u'\u1F31': {
            str += u'\u1F39';
        } break;
        case u'\u1F32': {
            str += u'\u1F3A';
        } break;
        case u'\u1F33': {
            str += u'\u1F3B';
        } break;
        case u'\u1F34': {
            str += u'\u1F3C';
        } break;
        case u'\u1F35': {
            str += u'\u1F3D';
        } break;
        case u'\u1F36': {
            str += u'\u1F3E';
        } break;
        case u'\u1F37': {
            str += u'\u1F3F';
        } break;
        case u'\u1F40': {
            str += u'\u1F48';
        } break;
        case u'\u1F41': {
            str += u'\u1F49';
        } break;
        case u'\u1F42': {
            str += u'\u1F4A';
        } break;
        case u'\u1F43': {
            str += u'\u1F4B';
        } break;
        case u'\u1F44': {
            str += u'\u1F4C';
        } break;
        case u'\u1F45': {
            str += u'\u1F4D';
        } break;
        case u'\u1F50': {
            str += u'\u03A5';
            str += u'\u0313';
        } break;
        case u'\u1F51': {
            str += u'\u1F59';
        } break;
        case u'\u1F52': {
            str += u'\u03A5';
            str += u'\u0313';
            str += u'\u0300';
        } break;
        case u'\u1F53': {
            str += u'\u1F5B';
        } break;
        case u'\u1F54': {
            str += u'\u03A5';
            str += u'\u0313';
            str += u'\u0301';
        } break;
        case u'\u1F55': {
            str += u'\u1F5D';
        } break;
        case u'\u1F56': {
            str += u'\u03A5';
            str += u'\u0313';
            str += u'\u0342';
        } break;
        case u'\u1F57': {
            str += u'\u1F5F';
        } break;
        case u'\u1F60': {
            str += u'\u1F68';
        } break;
        case u'\u1F61': {
            str += u'\u1F69';
        } break;
        case u'\u1F62': {
            str += u'\u1F6A';
        } break;
        case u'\u1F63': {
            str += u'\u1F6B';
        } break;
        case u'\u1F64': {
            str += u'\u1F6C';
        } break;
        case u'\u1F65': {
            str += u'\u1F6D';
        } break;
        case u'\u1F66': {
            str += u'\u1F6E';
        } break;
        case u'\u1F67': {
            str += u'\u1F6F';
        } break;
        case u'\u1F70': {
            str += u'\u1FBA';
        } break;
        case u'\u1F71': {
            str += u'\u1FBB';
        } break;
        case u'\u1F72': {
            str += u'\u1FC8';
        } break;
        case u'\u1F73': {
            str += u'\u1FC9';
        } break;
        case u'\u1F74': {
            str += u'\u1FCA';
        } break;
        case u'\u1F75': {
            str += u'\u1FCB';
        } break;
        case u'\u1F76': {
            str += u'\u1FDA';
        } break;
        case u'\u1F77': {
            str += u'\u1FDB';
        } break;
        case u'\u1F78': {
            str += u'\u1FF8';
        } break;
        case u'\u1F79': {
            str += u'\u1FF9';
        } break;
        case u'\u1F7A': {
            str += u'\u1FEA';
        } break;
        case u'\u1F7B': {
            str += u'\u1FEB';
        } break;
        case u'\u1F7C': {
            str += u'\u1FFA';
        } break;
        case u'\u1F7D': {
            str += u'\u1FFB';
        } break;
        case u'\u1F80': [[fallthrough]];
        case u'\u1F88': {
            str += u'\u1F08';
            str += u'\u0399';
        } break;
        case u'\u1F81': [[fallthrough]];
        case u'\u1F89': {
            str += u'\u1F09';
            str += u'\u0399';
        } break;
        case u'\u1F82': [[fallthrough]];
        case u'\u1F8A': {
            str += u'\u1F0A';
            str += u'\u0399';
        } break;
        case u'\u1F83': [[fallthrough]];
        case u'\u1F8B': {
            str += u'\u1F0B';
            str += u'\u0399';
        } break;
        case u'\u1F84': [[fallthrough]];
        case u'\u1F8C': {
            str += u'\u1F0C';
            str += u'\u0399';
        } break;
        case u'\u1F85': [[fallthrough]];
        case u'\u1F8D': {
            str += u'\u1F0D';
            str += u'\u0399';
        } break;
        case u'\u1F86': [[fallthrough]];
        case u'\u1F8E': {
            str += u'\u1F0E';
            str += u'\u0399';
        } break;
        case u'\u1F87': [[fallthrough]];
        case u'\u1F8F': {
            str += u'\u1F0F';
            str += u'\u0399';
        } break;
        case u'\u1F90': [[fallthrough]];
        case u'\u1F98': {
            str += u'\u1F28';
            str += u'\u0399';
        } break;
        case u'\u1F91': [[fallthrough]];
        case u'\u1F99': {
            str += u'\u1F29';
            str += u'\u0399';
        } break;
        case u'\u1F92': [[fallthrough]];
        case u'\u1F9A': {
            str += u'\u1F2A';
            str += u'\u0399';
        } break;
        case u'\u1F93': [[fallthrough]];
        case u'\u1F9B': {
            str += u'\u1F2B';
            str += u'\u0399';
        } break;
        case u'\u1F94': [[fallthrough]];
        case u'\u1F9C': {
            str += u'\u1F2C';
            str += u'\u0399';
        } break;
        case u'\u1F95': [[fallthrough]];
        case u'\u1F9D': {
            str += u'\u1F2D';
            str += u'\u0399';
        } break;
        case u'\u1F96': [[fallthrough]];
        case u'\u1F9E': {
            str += u'\u1F2E';
            str += u'\u0399';
        } break;
        case u'\u1F97': [[fallthrough]];
        case u'\u1F9F': {
            str += u'\u1F2F';
            str += u'\u0399';
        } break;
        case u'\u1FA0': [[fallthrough]];
        case u'\u1FA8': {
            str += u'\u1F68';
            str += u'\u0399';
        } break;
        case u'\u1FA1': [[fallthrough]];
        case u'\u1FA9': {
            str += u'\u1F69';
            str += u'\u0399';
        } break;
        case u'\u1FA2': [[fallthrough]];
        case u'\u1FAA': {
            str += u'\u1F6A';
            str += u'\u0399';
        } break;
        case u'\u1FA3': [[fallthrough]];
        case u'\u1FAB': {
            str += u'\u1F6B';
            str += u'\u0399';
        } break;
        case u'\u1FA4': [[fallthrough]];
        case u'\u1FAC': {
            str += u'\u1F6C';
            str += u'\u0399';
        } break;
        case u'\u1FA5': [[fallthrough]];
        case u'\u1FAD': {
            str += u'\u1F6D';
            str += u'\u0399';
        } break;
        case u'\u1FA6': [[fallthrough]];
        case u'\u1FAE': {
            str += u'\u1F6E';
            str += u'\u0399';
        } break;
        case u'\u1FA7': [[fallthrough]];
        case u'\u1FAF': {
            str += u'\u1F6F';
            str += u'\u0399';
        } break;
        case u'\u1FB0': {
            str += u'\u1FB8';
        } break;
        case u'\u1FB1': {
            str += u'\u1FB9';
        } break;
        case u'\u1FB2': {
            str += u'\u1FBA';
            str += u'\u0399';
        } break;
        case u'\u1FB3': [[fallthrough]];
        case u'\u1FBC': {
            str += u'\u0391';
            str += u'\u0399';
        } break;
        case u'\u1FB4': {
            str += u'\u0386';
            str += u'\u0399';
        } break;
        case u'\u1FB6': {
            str += u'\u0391';
            str += u'\u0342';
        } break;
        case u'\u1FB7': {
            str += u'\u0391';
            str += u'\u0342';
            str += u'\u0399';
        } break;
        case u'\u1FC2': {
            str += u'\u1FCA';
            str += u'\u0399';
        } break;
        case u'\u1FC3': [[fallthrough]];
        case u'\u1FCC': {
            str += u'\u0397';
            str += u'\u0399';
        } break;
        case u'\u1FC4': {
            str += u'\u0389';
            str += u'\u0399';
        } break;
        case u'\u1FC6': {
            str += u'\u0397';
            str += u'\u0342';
        } break;
        case u'\u1FC7': {
            str += u'\u0397';
            str += u'\u0342';
            str += u'\u0399';
        } break;
        case u'\u1FD0': {
            str += u'\u1FD8';
        } break;
        case u'\u1FD1': {
            str += u'\u1FD9';
        } break;
        case u'\u1FD2': {
            str += u'\u0399';
            str += u'\u0308';
            str += u'\u0300';
        } break;
        case u'\u1FD6': {
            str += u'\u0399';
            str += u'\u0342';
        } break;
        case u'\u1FD7': {
            str += u'\u0399';
            str += u'\u0308';
            str += u'\u0342';
        } break;
        case u'\u1FE0': {
            str += u'\u1FE8';
        } break;
        case u'\u1FE1': {
            str += u'\u1FE9';
        } break;
        case u'\u1FE2': {
            str += u'\u03A5';
            str += u'\u0308';
            str += u'\u0300';
        } break;
        case u'\u1FE4': {
            str += u'\u03A1';
            str += u'\u0313';
        } break;
        case u'\u1FE5': {
            str += u'\u1FEC';
        } break;
        case u'\u1FE6': {
            str += u'\u03A5';
            str += u'\u0342';
        } break;
        case u'\u1FE7': {
            str += u'\u03A5';
            str += u'\u0308';
            str += u'\u0342';
        } break;
        case u'\u1FF2': {
            str += u'\u1FFA';
            str += u'\u0399';
        } break;
        case u'\u1FF3': [[fallthrough]];
        case u'\u1FFC': {
            str += u'\u03A9';
            str += u'\u0399';
        } break;
        case u'\u1FF4': {
            str += u'\u038F';
            str += u'\u0399';
        } break;
        case u'\u1FF6': {
            str += u'\u03A9';
            str += u'\u0342';
        } break;
        case u'\u1FF7': {
            str += u'\u03A9';
            str += u'\u0342';
            str += u'\u0399';
        } break;
        case u'\u214E': {
            str += u'\u2132';
        } break;
        case u'\u2170': {
            str += u'\u2160';
        } break;
        case u'\u2171': {
            str += u'\u2161';
        } break;
        case u'\u2172': {
            str += u'\u2162';
        } break;
        case u'\u2173': {
            str += u'\u2163';
        } break;
        case u'\u2174': {
            str += u'\u2164';
        } break;
        case u'\u2175': {
            str += u'\u2165';
        } break;
        case u'\u2176': {
            str += u'\u2166';
        } break;
        case u'\u2177': {
            str += u'\u2167';
        } break;
        case u'\u2178': {
            str += u'\u2168';
        } break;
        case u'\u2179': {
            str += u'\u2169';
        } break;
        case u'\u217A': {
            str += u'\u216A';
        } break;
        case u'\u217B': {
            str += u'\u216B';
        } break;
        case u'\u217C': {
            str += u'\u216C';
        } break;
        case u'\u217D': {
            str += u'\u216D';
        } break;
        case u'\u217E': {
            str += u'\u216E';
        } break;
        case u'\u217F': {
            str += u'\u216F';
        } break;
        case u'\u2184': {
            str += u'\u2183';
        } break;
        case u'\u24D0': {
            str += u'\u24B6';
        } break;
        case u'\u24D1': {
            str += u'\u24B7';
        } break;
        case u'\u24D2': {
            str += u'\u24B8';
        } break;
        case u'\u24D3': {
            str += u'\u24B9';
        } break;
        case u'\u24D4': {
            str += u'\u24BA';
        } break;
        case u'\u24D5': {
            str += u'\u24BB';
        } break;
        case u'\u24D6': {
            str += u'\u24BC';
        } break;
        case u'\u24D7': {
            str += u'\u24BD';
        } break;
        case u'\u24D8': {
            str += u'\u24BE';
        } break;
        case u'\u24D9': {
            str += u'\u24BF';
        } break;
        case u'\u24DA': {
            str += u'\u24C0';
        } break;
        case u'\u24DB': {
            str += u'\u24C1';
        } break;
        case u'\u24DC': {
            str += u'\u24C2';
        } break;
        case u'\u24DD': {
            str += u'\u24C3';
        } break;
        case u'\u24DE': {
            str += u'\u24C4';
        } break;
        case u'\u24DF': {
            str += u'\u24C5';
        } break;
        case u'\u24E0': {
            str += u'\u24C6';
        } break;
        case u'\u24E1': {
            str += u'\u24C7';
        } break;
        case u'\u24E2': {
            str += u'\u24C8';
        } break;
        case u'\u24E3': {
            str += u'\u24C9';
        } break;
        case u'\u24E4': {
            str += u'\u24CA';
        } break;
        case u'\u24E5': {
            str += u'\u24CB';
        } break;
        case u'\u24E6': {
            str += u'\u24CC';
        } break;
        case u'\u24E7': {
            str += u'\u24CD';
        } break;
        case u'\u24E8': {
            str += u'\u24CE';
        } break;
        case u'\u24E9': {
            str += u'\u24CF';
        } break;
        case u'\u2C30': {
            str += u'\u2C00';
        } break;
        case u'\u2C31': {
            str += u'\u2C01';
        } break;
        case u'\u2C32': {
            str += u'\u2C02';
        } break;
        case u'\u2C33': {
            str += u'\u2C03';
        } break;
        case u'\u2C34': {
            str += u'\u2C04';
        } break;
        case u'\u2C35': {
            str += u'\u2C05';
        } break;
        case u'\u2C36': {
            str += u'\u2C06';
        } break;
        case u'\u2C37': {
            str += u'\u2C07';
        } break;
        case u'\u2C38': {
            str += u'\u2C08';
        } break;
        case u'\u2C39': {
            str += u'\u2C09';
        } break;
        case u'\u2C3A': {
            str += u'\u2C0A';
        } break;
        case u'\u2C3B': {
            str += u'\u2C0B';
        } break;
        case u'\u2C3C': {
            str += u'\u2C0C';
        } break;
        case u'\u2C3D': {
            str += u'\u2C0D';
        } break;
        case u'\u2C3E': {
            str += u'\u2C0E';
        } break;
        case u'\u2C3F': {
            str += u'\u2C0F';
        } break;
        case u'\u2C40': {
            str += u'\u2C10';
        } break;
        case u'\u2C41': {
            str += u'\u2C11';
        } break;
        case u'\u2C42': {
            str += u'\u2C12';
        } break;
        case u'\u2C43': {
            str += u'\u2C13';
        } break;
        case u'\u2C44': {
            str += u'\u2C14';
        } break;
        case u'\u2C45': {
            str += u'\u2C15';
        } break;
        case u'\u2C46': {
            str += u'\u2C16';
        } break;
        case u'\u2C47': {
            str += u'\u2C17';
        } break;
        case u'\u2C48': {
            str += u'\u2C18';
        } break;
        case u'\u2C49': {
            str += u'\u2C19';
        } break;
        case u'\u2C4A': {
            str += u'\u2C1A';
        } break;
        case u'\u2C4B': {
            str += u'\u2C1B';
        } break;
        case u'\u2C4C': {
            str += u'\u2C1C';
        } break;
        case u'\u2C4D': {
            str += u'\u2C1D';
        } break;
        case u'\u2C4E': {
            str += u'\u2C1E';
        } break;
        case u'\u2C4F': {
            str += u'\u2C1F';
        } break;
        case u'\u2C50': {
            str += u'\u2C20';
        } break;
        case u'\u2C51': {
            str += u'\u2C21';
        } break;
        case u'\u2C52': {
            str += u'\u2C22';
        } break;
        case u'\u2C53': {
            str += u'\u2C23';
        } break;
        case u'\u2C54': {
            str += u'\u2C24';
        } break;
        case u'\u2C55': {
            str += u'\u2C25';
        } break;
        case u'\u2C56': {
            str += u'\u2C26';
        } break;
        case u'\u2C57': {
            str += u'\u2C27';
        } break;
        case u'\u2C58': {
            str += u'\u2C28';
        } break;
        case u'\u2C59': {
            str += u'\u2C29';
        } break;
        case u'\u2C5A': {
            str += u'\u2C2A';
        } break;
        case u'\u2C5B': {
            str += u'\u2C2B';
        } break;
        case u'\u2C5C': {
            str += u'\u2C2C';
        } break;
        case u'\u2C5D': {
            str += u'\u2C2D';
        } break;
        case u'\u2C5E': {
            str += u'\u2C2E';
        } break;
        case u'\u2C5F': {
            str += u'\u2C2F';
        } break;
        case u'\u2C61': {
            str += u'\u2C60';
        } break;
        case u'\u2C65': {
            str += u'\u023A';
        } break;
        case u'\u2C66': {
            str += u'\u023E';
        } break;
        case u'\u2C68': {
            str += u'\u2C67';
        } break;
        case u'\u2C6A': {
            str += u'\u2C69';
        } break;
        case u'\u2C6C': {
            str += u'\u2C6B';
        } break;
        case u'\u2C73': {
            str += u'\u2C72';
        } break;
        case u'\u2C76': {
            str += u'\u2C75';
        } break;
        case u'\u2C81': {
            str += u'\u2C80';
        } break;
        case u'\u2C83': {
            str += u'\u2C82';
        } break;
        case u'\u2C85': {
            str += u'\u2C84';
        } break;
        case u'\u2C87': {
            str += u'\u2C86';
        } break;
        case u'\u2C89': {
            str += u'\u2C88';
        } break;
        case u'\u2C8B': {
            str += u'\u2C8A';
        } break;
        case u'\u2C8D': {
            str += u'\u2C8C';
        } break;
        case u'\u2C8F': {
            str += u'\u2C8E';
        } break;
        case u'\u2C91': {
            str += u'\u2C90';
        } break;
        case u'\u2C93': {
            str += u'\u2C92';
        } break;
        case u'\u2C95': {
            str += u'\u2C94';
        } break;
        case u'\u2C97': {
            str += u'\u2C96';
        } break;
        case u'\u2C99': {
            str += u'\u2C98';
        } break;
        case u'\u2C9B': {
            str += u'\u2C9A';
        } break;
        case u'\u2C9D': {
            str += u'\u2C9C';
        } break;
        case u'\u2C9F': {
            str += u'\u2C9E';
        } break;
        case u'\u2CA1': {
            str += u'\u2CA0';
        } break;
        case u'\u2CA3': {
            str += u'\u2CA2';
        } break;
        case u'\u2CA5': {
            str += u'\u2CA4';
        } break;
        case u'\u2CA7': {
            str += u'\u2CA6';
        } break;
        case u'\u2CA9': {
            str += u'\u2CA8';
        } break;
        case u'\u2CAB': {
            str += u'\u2CAA';
        } break;
        case u'\u2CAD': {
            str += u'\u2CAC';
        } break;
        case u'\u2CAF': {
            str += u'\u2CAE';
        } break;
        case u'\u2CB1': {
            str += u'\u2CB0';
        } break;
        case u'\u2CB3': {
            str += u'\u2CB2';
        } break;
        case u'\u2CB5': {
            str += u'\u2CB4';
        } break;
        case u'\u2CB7': {
            str += u'\u2CB6';
        } break;
        case u'\u2CB9': {
            str += u'\u2CB8';
        } break;
        case u'\u2CBB': {
            str += u'\u2CBA';
        } break;
        case u'\u2CBD': {
            str += u'\u2CBC';
        } break;
        case u'\u2CBF': {
            str += u'\u2CBE';
        } break;
        case u'\u2CC1': {
            str += u'\u2CC0';
        } break;
        case u'\u2CC3': {
            str += u'\u2CC2';
        } break;
        case u'\u2CC5': {
            str += u'\u2CC4';
        } break;
        case u'\u2CC7': {
            str += u'\u2CC6';
        } break;
        case u'\u2CC9': {
            str += u'\u2CC8';
        } break;
        case u'\u2CCB': {
            str += u'\u2CCA';
        } break;
        case u'\u2CCD': {
            str += u'\u2CCC';
        } break;
        case u'\u2CCF': {
            str += u'\u2CCE';
        } break;
        case u'\u2CD1': {
            str += u'\u2CD0';
        } break;
        case u'\u2CD3': {
            str += u'\u2CD2';
        } break;
        case u'\u2CD5': {
            str += u'\u2CD4';
        } break;
        case u'\u2CD7': {
            str += u'\u2CD6';
        } break;
        case u'\u2CD9': {
            str += u'\u2CD8';
        } break;
        case u'\u2CDB': {
            str += u'\u2CDA';
        } break;
        case u'\u2CDD': {
            str += u'\u2CDC';
        } break;
        case u'\u2CDF': {
            str += u'\u2CDE';
        } break;
        case u'\u2CE1': {
            str += u'\u2CE0';
        } break;
        case u'\u2CE3': {
            str += u'\u2CE2';
        } break;
        case u'\u2CEC': {
            str += u'\u2CEB';
        } break;
        case u'\u2CEE': {
            str += u'\u2CED';
        } break;
        case u'\u2CF3': {
            str += u'\u2CF2';
        } break;
        case u'\u2D00': {
            str += u'\u10A0';
        } break;
        case u'\u2D01': {
            str += u'\u10A1';
        } break;
        case u'\u2D02': {
            str += u'\u10A2';
        } break;
        case u'\u2D03': {
            str += u'\u10A3';
        } break;
        case u'\u2D04': {
            str += u'\u10A4';
        } break;
        case u'\u2D05': {
            str += u'\u10A5';
        } break;
        case u'\u2D06': {
            str += u'\u10A6';
        } break;
        case u'\u2D07': {
            str += u'\u10A7';
        } break;
        case u'\u2D08': {
            str += u'\u10A8';
        } break;
        case u'\u2D09': {
            str += u'\u10A9';
        } break;
        case u'\u2D0A': {
            str += u'\u10AA';
        } break;
        case u'\u2D0B': {
            str += u'\u10AB';
        } break;
        case u'\u2D0C': {
            str += u'\u10AC';
        } break;
        case u'\u2D0D': {
            str += u'\u10AD';
        } break;
        case u'\u2D0E': {
            str += u'\u10AE';
        } break;
        case u'\u2D0F': {
            str += u'\u10AF';
        } break;
        case u'\u2D10': {
            str += u'\u10B0';
        } break;
        case u'\u2D11': {
            str += u'\u10B1';
        } break;
        case u'\u2D12': {
            str += u'\u10B2';
        } break;
        case u'\u2D13': {
            str += u'\u10B3';
        } break;
        case u'\u2D14': {
            str += u'\u10B4';
        } break;
        case u'\u2D15': {
            str += u'\u10B5';
        } break;
        case u'\u2D16': {
            str += u'\u10B6';
        } break;
        case u'\u2D17': {
            str += u'\u10B7';
        } break;
        case u'\u2D18': {
            str += u'\u10B8';
        } break;
        case u'\u2D19': {
            str += u'\u10B9';
        } break;
        case u'\u2D1A': {
            str += u'\u10BA';
        } break;
        case u'\u2D1B': {
            str += u'\u10BB';
        } break;
        case u'\u2D1C': {
            str += u'\u10BC';
        } break;
        case u'\u2D1D': {
            str += u'\u10BD';
        } break;
        case u'\u2D1E': {
            str += u'\u10BE';
        } break;
        case u'\u2D1F': {
            str += u'\u10BF';
        } break;
        case u'\u2D20': {
            str += u'\u10C0';
        } break;
        case u'\u2D21': {
            str += u'\u10C1';
        } break;
        case u'\u2D22': {
            str += u'\u10C2';
        } break;
        case u'\u2D23': {
            str += u'\u10C3';
        } break;
        case u'\u2D24': {
            str += u'\u10C4';
        } break;
        case u'\u2D25': {
            str += u'\u10C5';
        } break;
        case u'\u2D27': {
            str += u'\u10C7';
        } break;
        case u'\u2D2D': {
            str += u'\u10CD';
        } break;
        case u'\uA641': {
            str += u'\uA640';
        } break;
        case u'\uA643': {
            str += u'\uA642';
        } break;
        case u'\uA645': {
            str += u'\uA644';
        } break;
        case u'\uA647': {
            str += u'\uA646';
        } break;
        case u'\uA649': {
            str += u'\uA648';
        } break;
        case u'\uA64D': {
            str += u'\uA64C';
        } break;
        case u'\uA64F': {
            str += u'\uA64E';
        } break;
        case u'\uA651': {
            str += u'\uA650';
        } break;
        case u'\uA653': {
            str += u'\uA652';
        } break;
        case u'\uA655': {
            str += u'\uA654';
        } break;
        case u'\uA657': {
            str += u'\uA656';
        } break;
        case u'\uA659': {
            str += u'\uA658';
        } break;
        case u'\uA65B': {
            str += u'\uA65A';
        } break;
        case u'\uA65D': {
            str += u'\uA65C';
        } break;
        case u'\uA65F': {
            str += u'\uA65E';
        } break;
        case u'\uA661': {
            str += u'\uA660';
        } break;
        case u'\uA663': {
            str += u'\uA662';
        } break;
        case u'\uA665': {
            str += u'\uA664';
        } break;
        case u'\uA667': {
            str += u'\uA666';
        } break;
        case u'\uA669': {
            str += u'\uA668';
        } break;
        case u'\uA66B': {
            str += u'\uA66A';
        } break;
        case u'\uA66D': {
            str += u'\uA66C';
        } break;
        case u'\uA681': {
            str += u'\uA680';
        } break;
        case u'\uA683': {
            str += u'\uA682';
        } break;
        case u'\uA685': {
            str += u'\uA684';
        } break;
        case u'\uA687': {
            str += u'\uA686';
        } break;
        case u'\uA689': {
            str += u'\uA688';
        } break;
        case u'\uA68B': {
            str += u'\uA68A';
        } break;
        case u'\uA68D': {
            str += u'\uA68C';
        } break;
        case u'\uA68F': {
            str += u'\uA68E';
        } break;
        case u'\uA691': {
            str += u'\uA690';
        } break;
        case u'\uA693': {
            str += u'\uA692';
        } break;
        case u'\uA695': {
            str += u'\uA694';
        } break;
        case u'\uA697': {
            str += u'\uA696';
        } break;
        case u'\uA699': {
            str += u'\uA698';
        } break;
        case u'\uA69B': {
            str += u'\uA69A';
        } break;
        case u'\uA723': {
            str += u'\uA722';
        } break;
        case u'\uA725': {
            str += u'\uA724';
        } break;
        case u'\uA727': {
            str += u'\uA726';
        } break;
        case u'\uA729': {
            str += u'\uA728';
        } break;
        case u'\uA72B': {
            str += u'\uA72A';
        } break;
        case u'\uA72D': {
            str += u'\uA72C';
        } break;
        case u'\uA72F': {
            str += u'\uA72E';
        } break;
        case u'\uA733': {
            str += u'\uA732';
        } break;
        case u'\uA735': {
            str += u'\uA734';
        } break;
        case u'\uA737': {
            str += u'\uA736';
        } break;
        case u'\uA739': {
            str += u'\uA738';
        } break;
        case u'\uA73B': {
            str += u'\uA73A';
        } break;
        case u'\uA73D': {
            str += u'\uA73C';
        } break;
        case u'\uA73F': {
            str += u'\uA73E';
        } break;
        case u'\uA741': {
            str += u'\uA740';
        } break;
        case u'\uA743': {
            str += u'\uA742';
        } break;
        case u'\uA745': {
            str += u'\uA744';
        } break;
        case u'\uA747': {
            str += u'\uA746';
        } break;
        case u'\uA749': {
            str += u'\uA748';
        } break;
        case u'\uA74B': {
            str += u'\uA74A';
        } break;
        case u'\uA74D': {
            str += u'\uA74C';
        } break;
        case u'\uA74F': {
            str += u'\uA74E';
        } break;
        case u'\uA751': {
            str += u'\uA750';
        } break;
        case u'\uA753': {
            str += u'\uA752';
        } break;
        case u'\uA755': {
            str += u'\uA754';
        } break;
        case u'\uA757': {
            str += u'\uA756';
        } break;
        case u'\uA759': {
            str += u'\uA758';
        } break;
        case u'\uA75B': {
            str += u'\uA75A';
        } break;
        case u'\uA75D': {
            str += u'\uA75C';
        } break;
        case u'\uA75F': {
            str += u'\uA75E';
        } break;
        case u'\uA761': {
            str += u'\uA760';
        } break;
        case u'\uA763': {
            str += u'\uA762';
        } break;
        case u'\uA765': {
            str += u'\uA764';
        } break;
        case u'\uA767': {
            str += u'\uA766';
        } break;
        case u'\uA769': {
            str += u'\uA768';
        } break;
        case u'\uA76B': {
            str += u'\uA76A';
        } break;
        case u'\uA76D': {
            str += u'\uA76C';
        } break;
        case u'\uA76F': {
            str += u'\uA76E';
        } break;
        case u'\uA77A': {
            str += u'\uA779';
        } break;
        case u'\uA77C': {
            str += u'\uA77B';
        } break;
        case u'\uA77F': {
            str += u'\uA77E';
        } break;
        case u'\uA781': {
            str += u'\uA780';
        } break;
        case u'\uA783': {
            str += u'\uA782';
        } break;
        case u'\uA785': {
            str += u'\uA784';
        } break;
        case u'\uA787': {
            str += u'\uA786';
        } break;
        case u'\uA78C': {
            str += u'\uA78B';
        } break;
        case u'\uA791': {
            str += u'\uA790';
        } break;
        case u'\uA793': {
            str += u'\uA792';
        } break;
        case u'\uA794': {
            str += u'\uA7C4';
        } break;
        case u'\uA797': {
            str += u'\uA796';
        } break;
        case u'\uA799': {
            str += u'\uA798';
        } break;
        case u'\uA79B': {
            str += u'\uA79A';
        } break;
        case u'\uA79D': {
            str += u'\uA79C';
        } break;
        case u'\uA79F': {
            str += u'\uA79E';
        } break;
        case u'\uA7A1': {
            str += u'\uA7A0';
        } break;
        case u'\uA7A3': {
            str += u'\uA7A2';
        } break;
        case u'\uA7A5': {
            str += u'\uA7A4';
        } break;
        case u'\uA7A7': {
            str += u'\uA7A6';
        } break;
        case u'\uA7A9': {
            str += u'\uA7A8';
        } break;
        case u'\uA7B5': {
            str += u'\uA7B4';
        } break;
        case u'\uA7B7': {
            str += u'\uA7B6';
        } break;
        case u'\uA7B9': {
            str += u'\uA7B8';
        } break;
        case u'\uA7BB': {
            str += u'\uA7BA';
        } break;
        case u'\uA7BD': {
            str += u'\uA7BC';
        } break;
        case u'\uA7BF': {
            str += u'\uA7BE';
        } break;
        case u'\uA7C1': {
            str += u'\uA7C0';
        } break;
        case u'\uA7C3': {
            str += u'\uA7C2';
        } break;
        case u'\uA7C8': {
            str += u'\uA7C7';
        } break;
        case u'\uA7CA': {
            str += u'\uA7C9';
        } break;
        case u'\uA7D1': {
            str += u'\uA7D0';
        } break;
        case u'\uA7D7': {
            str += u'\uA7D6';
        } break;
        case u'\uA7D9': {
            str += u'\uA7D8';
        } break;
        case u'\uA7F6': {
            str += u'\uA7F5';
        } break;
        case u'\uAB53': {
            str += u'\uA7B3';
        } break;
        case u'\uAB70': {
            str += u'\u13A0';
        } break;
        case u'\uAB71': {
            str += u'\u13A1';
        } break;
        case u'\uAB72': {
            str += u'\u13A2';
        } break;
        case u'\uAB73': {
            str += u'\u13A3';
        } break;
        case u'\uAB74': {
            str += u'\u13A4';
        } break;
        case u'\uAB75': {
            str += u'\u13A5';
        } break;
        case u'\uAB76': {
            str += u'\u13A6';
        } break;
        case u'\uAB77': {
            str += u'\u13A7';
        } break;
        case u'\uAB78': {
            str += u'\u13A8';
        } break;
        case u'\uAB79': {
            str += u'\u13A9';
        } break;
        case u'\uAB7A': {
            str += u'\u13AA';
        } break;
        case u'\uAB7B': {
            str += u'\u13AB';
        } break;
        case u'\uAB7C': {
            str += u'\u13AC';
        } break;
        case u'\uAB7D': {
            str += u'\u13AD';
        } break;
        case u'\uAB7E': {
            str += u'\u13AE';
        } break;
        case u'\uAB7F': {
            str += u'\u13AF';
        } break;
        case u'\uAB80': {
            str += u'\u13B0';
        } break;
        case u'\uAB81': {
            str += u'\u13B1';
        } break;
        case u'\uAB82': {
            str += u'\u13B2';
        } break;
        case u'\uAB83': {
            str += u'\u13B3';
        } break;
        case u'\uAB84': {
            str += u'\u13B4';
        } break;
        case u'\uAB85': {
            str += u'\u13B5';
        } break;
        case u'\uAB86': {
            str += u'\u13B6';
        } break;
        case u'\uAB87': {
            str += u'\u13B7';
        } break;
        case u'\uAB88': {
            str += u'\u13B8';
        } break;
        case u'\uAB89': {
            str += u'\u13B9';
        } break;
        case u'\uAB8A': {
            str += u'\u13BA';
        } break;
        case u'\uAB8B': {
            str += u'\u13BB';
        } break;
        case u'\uAB8C': {
            str += u'\u13BC';
        } break;
        case u'\uAB8D': {
            str += u'\u13BD';
        } break;
        case u'\uAB8E': {
            str += u'\u13BE';
        } break;
        case u'\uAB8F': {
            str += u'\u13BF';
        } break;
        case u'\uAB90': {
            str += u'\u13C0';
        } break;
        case u'\uAB91': {
            str += u'\u13C1';
        } break;
        case u'\uAB92': {
            str += u'\u13C2';
        } break;
        case u'\uAB93': {
            str += u'\u13C3';
        } break;
        case u'\uAB94': {
            str += u'\u13C4';
        } break;
        case u'\uAB95': {
            str += u'\u13C5';
        } break;
        case u'\uAB96': {
            str += u'\u13C6';
        } break;
        case u'\uAB97': {
            str += u'\u13C7';
        } break;
        case u'\uAB98': {
            str += u'\u13C8';
        } break;
        case u'\uAB99': {
            str += u'\u13C9';
        } break;
        case u'\uAB9A': {
            str += u'\u13CA';
        } break;
        case u'\uAB9B': {
            str += u'\u13CB';
        } break;
        case u'\uAB9C': {
            str += u'\u13CC';
        } break;
        case u'\uAB9D': {
            str += u'\u13CD';
        } break;
        case u'\uAB9E': {
            str += u'\u13CE';
        } break;
        case u'\uAB9F': {
            str += u'\u13CF';
        } break;
        case u'\uABA0': {
            str += u'\u13D0';
        } break;
        case u'\uABA1': {
            str += u'\u13D1';
        } break;
        case u'\uABA2': {
            str += u'\u13D2';
        } break;
        case u'\uABA3': {
            str += u'\u13D3';
        } break;
        case u'\uABA4': {
            str += u'\u13D4';
        } break;
        case u'\uABA5': {
            str += u'\u13D5';
        } break;
        case u'\uABA6': {
            str += u'\u13D6';
        } break;
        case u'\uABA7': {
            str += u'\u13D7';
        } break;
        case u'\uABA8': {
            str += u'\u13D8';
        } break;
        case u'\uABA9': {
            str += u'\u13D9';
        } break;
        case u'\uABAA': {
            str += u'\u13DA';
        } break;
        case u'\uABAB': {
            str += u'\u13DB';
        } break;
        case u'\uABAC': {
            str += u'\u13DC';
        } break;
        case u'\uABAD': {
            str += u'\u13DD';
        } break;
        case u'\uABAE': {
            str += u'\u13DE';
        } break;
        case u'\uABAF': {
            str += u'\u13DF';
        } break;
        case u'\uABB0': {
            str += u'\u13E0';
        } break;
        case u'\uABB1': {
            str += u'\u13E1';
        } break;
        case u'\uABB2': {
            str += u'\u13E2';
        } break;
        case u'\uABB3': {
            str += u'\u13E3';
        } break;
        case u'\uABB4': {
            str += u'\u13E4';
        } break;
        case u'\uABB5': {
            str += u'\u13E5';
        } break;
        case u'\uABB6': {
            str += u'\u13E6';
        } break;
        case u'\uABB7': {
            str += u'\u13E7';
        } break;
        case u'\uABB8': {
            str += u'\u13E8';
        } break;
        case u'\uABB9': {
            str += u'\u13E9';
        } break;
        case u'\uABBA': {
            str += u'\u13EA';
        } break;
        case u'\uABBB': {
            str += u'\u13EB';
        } break;
        case u'\uABBC': {
            str += u'\u13EC';
        } break;
        case u'\uABBD': {
            str += u'\u13ED';
        } break;
        case u'\uABBE': {
            str += u'\u13EE';
        } break;
        case u'\uABBF': {
            str += u'\u13EF';
        } break;
        case u'\uFB00': {
            str += u'\u0046';
            str += u'\u0046';
        } break;
        case u'\uFB01': {
            str += u'\u0046';
            str += u'\u0049';
        } break;
        case u'\uFB02': {
            str += u'\u0046';
            str += u'\u004C';
        } break;
        case u'\uFB03': {
            str += u'\u0046';
            str += u'\u0046';
            str += u'\u0049';
        } break;
        case u'\uFB04': {
            str += u'\u0046';
            str += u'\u0046';
            str += u'\u004C';
        } break;
        case u'\uFB05': [[fallthrough]];
        case u'\uFB06': {
            str += u'\u0053';
            str += u'\u0054';
        } break;
        case u'\uFB13': {
            str += u'\u0544';
            str += u'\u0546';
        } break;
        case u'\uFB14': {
            str += u'\u0544';
            str += u'\u0535';
        } break;
        case u'\uFB15': {
            str += u'\u0544';
            str += u'\u053B';
        } break;
        case u'\uFB16': {
            str += u'\u054E';
            str += u'\u0546';
        } break;
        case u'\uFB17': {
            str += u'\u0544';
            str += u'\u053D';
        } break;
        case u'\uFF41': {
            str += u'\uFF21';
        } break;
        case u'\uFF42': {
            str += u'\uFF22';
        } break;
        case u'\uFF43': {
            str += u'\uFF23';
        } break;
        case u'\uFF44': {
            str += u'\uFF24';
        } break;
        case u'\uFF45': {
            str += u'\uFF25';
        } break;
        case u'\uFF46': {
            str += u'\uFF26';
        } break;
        case u'\uFF47': {
            str += u'\uFF27';
        } break;
        case u'\uFF48': {
            str += u'\uFF28';
        } break;
        case u'\uFF49': {
            str += u'\uFF29';
        } break;
        case u'\uFF4A': {
            str += u'\uFF2A';
        } break;
        case u'\uFF4B': {
            str += u'\uFF2B';
        } break;
        case u'\uFF4C': {
            str += u'\uFF2C';
        } break;
        case u'\uFF4D': {
            str += u'\uFF2D';
        } break;
        case u'\uFF4E': {
            str += u'\uFF2E';
        } break;
        case u'\uFF4F': {
            str += u'\uFF2F';
        } break;
        case u'\uFF50': {
            str += u'\uFF30';
        } break;
        case u'\uFF51': {
            str += u'\uFF31';
        } break;
        case u'\uFF52': {
            str += u'\uFF32';
        } break;
        case u'\uFF53': {
            str += u'\uFF33';
        } break;
        case u'\uFF54': {
            str += u'\uFF34';
        } break;
        case u'\uFF55': {
            str += u'\uFF35';
        } break;
        case u'\uFF56': {
            str += u'\uFF36';
        } break;
        case u'\uFF57': {
            str += u'\uFF37';
        } break;
        case u'\uFF58': {
            str += u'\uFF38';
        } break;
        case u'\uFF59': {
            str += u'\uFF39';
        } break;
        case u'\uFF5A': {
            str += u'\uFF3A';
        } break;
        default: {
            str += ch;
        } break;
    }
}

void Unicode::down(String& str, char16 ch) {
    switch (ch) {
        case u'\u0041': {
            str += u'\u0061';
        } break;
        case u'\u0042': {
            str += u'\u0062';
        } break;
        case u'\u0043': {
            str += u'\u0063';
        } break;
        case u'\u0044': {
            str += u'\u0064';
        } break;
        case u'\u0045': {
            str += u'\u0065';
        } break;
        case u'\u0046': {
            str += u'\u0066';
        } break;
        case u'\u0047': {
            str += u'\u0067';
        } break;
        case u'\u0048': {
            str += u'\u0068';
        } break;
        case u'\u0049': {
            str += u'\u0069';
        } break;
        case u'\u004A': {
            str += u'\u006A';
        } break;
        case u'\u004B': [[fallthrough]];
        case u'\u212A': {
            str += u'\u006B';
        } break;
        case u'\u004C': {
            str += u'\u006C';
        } break;
        case u'\u004D': {
            str += u'\u006D';
        } break;
        case u'\u004E': {
            str += u'\u006E';
        } break;
        case u'\u004F': {
            str += u'\u006F';
        } break;
        case u'\u0050': {
            str += u'\u0070';
        } break;
        case u'\u0051': {
            str += u'\u0071';
        } break;
        case u'\u0052': {
            str += u'\u0072';
        } break;
        case u'\u0053': {
            str += u'\u0073';
        } break;
        case u'\u0054': {
            str += u'\u0074';
        } break;
        case u'\u0055': {
            str += u'\u0075';
        } break;
        case u'\u0056': {
            str += u'\u0076';
        } break;
        case u'\u0057': {
            str += u'\u0077';
        } break;
        case u'\u0058': {
            str += u'\u0078';
        } break;
        case u'\u0059': {
            str += u'\u0079';
        } break;
        case u'\u005A': {
            str += u'\u007A';
        } break;
        case u'\u00C0': {
            str += u'\u00E0';
        } break;
        case u'\u00C1': {
            str += u'\u00E1';
        } break;
        case u'\u00C2': {
            str += u'\u00E2';
        } break;
        case u'\u00C3': {
            str += u'\u00E3';
        } break;
        case u'\u00C4': {
            str += u'\u00E4';
        } break;
        case u'\u00C5': [[fallthrough]];
        case u'\u212B': {
            str += u'\u00E5';
        } break;
        case u'\u00C6': {
            str += u'\u00E6';
        } break;
        case u'\u00C7': {
            str += u'\u00E7';
        } break;
        case u'\u00C8': {
            str += u'\u00E8';
        } break;
        case u'\u00C9': {
            str += u'\u00E9';
        } break;
        case u'\u00CA': {
            str += u'\u00EA';
        } break;
        case u'\u00CB': {
            str += u'\u00EB';
        } break;
        case u'\u00CC': {
            str += u'\u00EC';
        } break;
        case u'\u00CD': {
            str += u'\u00ED';
        } break;
        case u'\u00CE': {
            str += u'\u00EE';
        } break;
        case u'\u00CF': {
            str += u'\u00EF';
        } break;
        case u'\u00D0': {
            str += u'\u00F0';
        } break;
        case u'\u00D1': {
            str += u'\u00F1';
        } break;
        case u'\u00D2': {
            str += u'\u00F2';
        } break;
        case u'\u00D3': {
            str += u'\u00F3';
        } break;
        case u'\u00D4': {
            str += u'\u00F4';
        } break;
        case u'\u00D5': {
            str += u'\u00F5';
        } break;
        case u'\u00D6': {
            str += u'\u00F6';
        } break;
        case u'\u00D8': {
            str += u'\u00F8';
        } break;
        case u'\u00D9': {
            str += u'\u00F9';
        } break;
        case u'\u00DA': {
            str += u'\u00FA';
        } break;
        case u'\u00DB': {
            str += u'\u00FB';
        } break;
        case u'\u00DC': {
            str += u'\u00FC';
        } break;
        case u'\u00DD': {
            str += u'\u00FD';
        } break;
        case u'\u00DE': {
            str += u'\u00FE';
        } break;
        case u'\u0100': {
            str += u'\u0101';
        } break;
        case u'\u0102': {
            str += u'\u0103';
        } break;
        case u'\u0104': {
            str += u'\u0105';
        } break;
        case u'\u0106': {
            str += u'\u0107';
        } break;
        case u'\u0108': {
            str += u'\u0109';
        } break;
        case u'\u010A': {
            str += u'\u010B';
        } break;
        case u'\u010C': {
            str += u'\u010D';
        } break;
        case u'\u010E': {
            str += u'\u010F';
        } break;
        case u'\u0110': {
            str += u'\u0111';
        } break;
        case u'\u0112': {
            str += u'\u0113';
        } break;
        case u'\u0114': {
            str += u'\u0115';
        } break;
        case u'\u0116': {
            str += u'\u0117';
        } break;
        case u'\u0118': {
            str += u'\u0119';
        } break;
        case u'\u011A': {
            str += u'\u011B';
        } break;
        case u'\u011C': {
            str += u'\u011D';
        } break;
        case u'\u011E': {
            str += u'\u011F';
        } break;
        case u'\u0120': {
            str += u'\u0121';
        } break;
        case u'\u0122': {
            str += u'\u0123';
        } break;
        case u'\u0124': {
            str += u'\u0125';
        } break;
        case u'\u0126': {
            str += u'\u0127';
        } break;
        case u'\u0128': {
            str += u'\u0129';
        } break;
        case u'\u012A': {
            str += u'\u012B';
        } break;
        case u'\u012C': {
            str += u'\u012D';
        } break;
        case u'\u012E': {
            str += u'\u012F';
        } break;
        case u'\u0130': {
            str += u'\u0069';
            str += u'\u0307';
        } break;
        case u'\u0132': {
            str += u'\u0133';
        } break;
        case u'\u0134': {
            str += u'\u0135';
        } break;
        case u'\u0136': {
            str += u'\u0137';
        } break;
        case u'\u0139': {
            str += u'\u013A';
        } break;
        case u'\u013B': {
            str += u'\u013C';
        } break;
        case u'\u013D': {
            str += u'\u013E';
        } break;
        case u'\u013F': {
            str += u'\u0140';
        } break;
        case u'\u0141': {
            str += u'\u0142';
        } break;
        case u'\u0143': {
            str += u'\u0144';
        } break;
        case u'\u0145': {
            str += u'\u0146';
        } break;
        case u'\u0147': {
            str += u'\u0148';
        } break;
        case u'\u014A': {
            str += u'\u014B';
        } break;
        case u'\u014C': {
            str += u'\u014D';
        } break;
        case u'\u014E': {
            str += u'\u014F';
        } break;
        case u'\u0150': {
            str += u'\u0151';
        } break;
        case u'\u0152': {
            str += u'\u0153';
        } break;
        case u'\u0154': {
            str += u'\u0155';
        } break;
        case u'\u0156': {
            str += u'\u0157';
        } break;
        case u'\u0158': {
            str += u'\u0159';
        } break;
        case u'\u015A': {
            str += u'\u015B';
        } break;
        case u'\u015C': {
            str += u'\u015D';
        } break;
        case u'\u015E': {
            str += u'\u015F';
        } break;
        case u'\u0160': {
            str += u'\u0161';
        } break;
        case u'\u0162': {
            str += u'\u0163';
        } break;
        case u'\u0164': {
            str += u'\u0165';
        } break;
        case u'\u0166': {
            str += u'\u0167';
        } break;
        case u'\u0168': {
            str += u'\u0169';
        } break;
        case u'\u016A': {
            str += u'\u016B';
        } break;
        case u'\u016C': {
            str += u'\u016D';
        } break;
        case u'\u016E': {
            str += u'\u016F';
        } break;
        case u'\u0170': {
            str += u'\u0171';
        } break;
        case u'\u0172': {
            str += u'\u0173';
        } break;
        case u'\u0174': {
            str += u'\u0175';
        } break;
        case u'\u0176': {
            str += u'\u0177';
        } break;
        case u'\u0178': {
            str += u'\u00FF';
        } break;
        case u'\u0179': {
            str += u'\u017A';
        } break;
        case u'\u017B': {
            str += u'\u017C';
        } break;
        case u'\u017D': {
            str += u'\u017E';
        } break;
        case u'\u0181': {
            str += u'\u0253';
        } break;
        case u'\u0182': {
            str += u'\u0183';
        } break;
        case u'\u0184': {
            str += u'\u0185';
        } break;
        case u'\u0186': {
            str += u'\u0254';
        } break;
        case u'\u0187': {
            str += u'\u0188';
        } break;
        case u'\u0189': {
            str += u'\u0256';
        } break;
        case u'\u018A': {
            str += u'\u0257';
        } break;
        case u'\u018B': {
            str += u'\u018C';
        } break;
        case u'\u018E': {
            str += u'\u01DD';
        } break;
        case u'\u018F': {
            str += u'\u0259';
        } break;
        case u'\u0190': {
            str += u'\u025B';
        } break;
        case u'\u0191': {
            str += u'\u0192';
        } break;
        case u'\u0193': {
            str += u'\u0260';
        } break;
        case u'\u0194': {
            str += u'\u0263';
        } break;
        case u'\u0196': {
            str += u'\u0269';
        } break;
        case u'\u0197': {
            str += u'\u0268';
        } break;
        case u'\u0198': {
            str += u'\u0199';
        } break;
        case u'\u019C': {
            str += u'\u026F';
        } break;
        case u'\u019D': {
            str += u'\u0272';
        } break;
        case u'\u019F': {
            str += u'\u0275';
        } break;
        case u'\u01A0': {
            str += u'\u01A1';
        } break;
        case u'\u01A2': {
            str += u'\u01A3';
        } break;
        case u'\u01A4': {
            str += u'\u01A5';
        } break;
        case u'\u01A6': {
            str += u'\u0280';
        } break;
        case u'\u01A7': {
            str += u'\u01A8';
        } break;
        case u'\u01A9': {
            str += u'\u0283';
        } break;
        case u'\u01AC': {
            str += u'\u01AD';
        } break;
        case u'\u01AE': {
            str += u'\u0288';
        } break;
        case u'\u01AF': {
            str += u'\u01B0';
        } break;
        case u'\u01B1': {
            str += u'\u028A';
        } break;
        case u'\u01B2': {
            str += u'\u028B';
        } break;
        case u'\u01B3': {
            str += u'\u01B4';
        } break;
        case u'\u01B5': {
            str += u'\u01B6';
        } break;
        case u'\u01B7': {
            str += u'\u0292';
        } break;
        case u'\u01B8': {
            str += u'\u01B9';
        } break;
        case u'\u01BC': {
            str += u'\u01BD';
        } break;
        case u'\u01C4': [[fallthrough]];
        case u'\u01C5': {
            str += u'\u01C6';
        } break;
        case u'\u01C7': [[fallthrough]];
        case u'\u01C8': {
            str += u'\u01C9';
        } break;
        case u'\u01CA': [[fallthrough]];
        case u'\u01CB': {
            str += u'\u01CC';
        } break;
        case u'\u01CD': {
            str += u'\u01CE';
        } break;
        case u'\u01CF': {
            str += u'\u01D0';
        } break;
        case u'\u01D1': {
            str += u'\u01D2';
        } break;
        case u'\u01D3': {
            str += u'\u01D4';
        } break;
        case u'\u01D5': {
            str += u'\u01D6';
        } break;
        case u'\u01D7': {
            str += u'\u01D8';
        } break;
        case u'\u01D9': {
            str += u'\u01DA';
        } break;
        case u'\u01DB': {
            str += u'\u01DC';
        } break;
        case u'\u01DE': {
            str += u'\u01DF';
        } break;
        case u'\u01E0': {
            str += u'\u01E1';
        } break;
        case u'\u01E2': {
            str += u'\u01E3';
        } break;
        case u'\u01E4': {
            str += u'\u01E5';
        } break;
        case u'\u01E6': {
            str += u'\u01E7';
        } break;
        case u'\u01E8': {
            str += u'\u01E9';
        } break;
        case u'\u01EA': {
            str += u'\u01EB';
        } break;
        case u'\u01EC': {
            str += u'\u01ED';
        } break;
        case u'\u01EE': {
            str += u'\u01EF';
        } break;
        case u'\u01F1': [[fallthrough]];
        case u'\u01F2': {
            str += u'\u01F3';
        } break;
        case u'\u01F4': {
            str += u'\u01F5';
        } break;
        case u'\u01F6': {
            str += u'\u0195';
        } break;
        case u'\u01F7': {
            str += u'\u01BF';
        } break;
        case u'\u01F8': {
            str += u'\u01F9';
        } break;
        case u'\u01FA': {
            str += u'\u01FB';
        } break;
        case u'\u01FC': {
            str += u'\u01FD';
        } break;
        case u'\u01FE': {
            str += u'\u01FF';
        } break;
        case u'\u0200': {
            str += u'\u0201';
        } break;
        case u'\u0202': {
            str += u'\u0203';
        } break;
        case u'\u0204': {
            str += u'\u0205';
        } break;
        case u'\u0206': {
            str += u'\u0207';
        } break;
        case u'\u0208': {
            str += u'\u0209';
        } break;
        case u'\u020A': {
            str += u'\u020B';
        } break;
        case u'\u020C': {
            str += u'\u020D';
        } break;
        case u'\u020E': {
            str += u'\u020F';
        } break;
        case u'\u0210': {
            str += u'\u0211';
        } break;
        case u'\u0212': {
            str += u'\u0213';
        } break;
        case u'\u0214': {
            str += u'\u0215';
        } break;
        case u'\u0216': {
            str += u'\u0217';
        } break;
        case u'\u0218': {
            str += u'\u0219';
        } break;
        case u'\u021A': {
            str += u'\u021B';
        } break;
        case u'\u021C': {
            str += u'\u021D';
        } break;
        case u'\u021E': {
            str += u'\u021F';
        } break;
        case u'\u0220': {
            str += u'\u019E';
        } break;
        case u'\u0222': {
            str += u'\u0223';
        } break;
        case u'\u0224': {
            str += u'\u0225';
        } break;
        case u'\u0226': {
            str += u'\u0227';
        } break;
        case u'\u0228': {
            str += u'\u0229';
        } break;
        case u'\u022A': {
            str += u'\u022B';
        } break;
        case u'\u022C': {
            str += u'\u022D';
        } break;
        case u'\u022E': {
            str += u'\u022F';
        } break;
        case u'\u0230': {
            str += u'\u0231';
        } break;
        case u'\u0232': {
            str += u'\u0233';
        } break;
        case u'\u023A': {
            str += u'\u2C65';
        } break;
        case u'\u023B': {
            str += u'\u023C';
        } break;
        case u'\u023D': {
            str += u'\u019A';
        } break;
        case u'\u023E': {
            str += u'\u2C66';
        } break;
        case u'\u0241': {
            str += u'\u0242';
        } break;
        case u'\u0243': {
            str += u'\u0180';
        } break;
        case u'\u0244': {
            str += u'\u0289';
        } break;
        case u'\u0245': {
            str += u'\u028C';
        } break;
        case u'\u0246': {
            str += u'\u0247';
        } break;
        case u'\u0248': {
            str += u'\u0249';
        } break;
        case u'\u024A': {
            str += u'\u024B';
        } break;
        case u'\u024C': {
            str += u'\u024D';
        } break;
        case u'\u024E': {
            str += u'\u024F';
        } break;
        case u'\u0370': {
            str += u'\u0371';
        } break;
        case u'\u0372': {
            str += u'\u0373';
        } break;
        case u'\u0376': {
            str += u'\u0377';
        } break;
        case u'\u037F': {
            str += u'\u03F3';
        } break;
        case u'\u0386': {
            str += u'\u03AC';
        } break;
        case u'\u0388': {
            str += u'\u03AD';
        } break;
        case u'\u0389': {
            str += u'\u03AE';
        } break;
        case u'\u038A': {
            str += u'\u03AF';
        } break;
        case u'\u038C': {
            str += u'\u03CC';
        } break;
        case u'\u038E': {
            str += u'\u03CD';
        } break;
        case u'\u038F': {
            str += u'\u03CE';
        } break;
        case u'\u0391': {
            str += u'\u03B1';
        } break;
        case u'\u0392': {
            str += u'\u03B2';
        } break;
        case u'\u0393': {
            str += u'\u03B3';
        } break;
        case u'\u0394': {
            str += u'\u03B4';
        } break;
        case u'\u0395': {
            str += u'\u03B5';
        } break;
        case u'\u0396': {
            str += u'\u03B6';
        } break;
        case u'\u0397': {
            str += u'\u03B7';
        } break;
        case u'\u0398': [[fallthrough]];
        case u'\u03F4': {
            str += u'\u03B8';
        } break;
        case u'\u0399': {
            str += u'\u03B9';
        } break;
        case u'\u039A': {
            str += u'\u03BA';
        } break;
        case u'\u039B': {
            str += u'\u03BB';
        } break;
        case u'\u039C': {
            str += u'\u03BC';
        } break;
        case u'\u039D': {
            str += u'\u03BD';
        } break;
        case u'\u039E': {
            str += u'\u03BE';
        } break;
        case u'\u039F': {
            str += u'\u03BF';
        } break;
        case u'\u03A0': {
            str += u'\u03C0';
        } break;
        case u'\u03A1': {
            str += u'\u03C1';
        } break;
        case u'\u03A3': {
            str += u'\u03C3';
        } break;
        case u'\u03A4': {
            str += u'\u03C4';
        } break;
        case u'\u03A5': {
            str += u'\u03C5';
        } break;
        case u'\u03A6': {
            str += u'\u03C6';
        } break;
        case u'\u03A7': {
            str += u'\u03C7';
        } break;
        case u'\u03A8': {
            str += u'\u03C8';
        } break;
        case u'\u03A9': [[fallthrough]];
        case u'\u2126': {
            str += u'\u03C9';
        } break;
        case u'\u03AA': {
            str += u'\u03CA';
        } break;
        case u'\u03AB': {
            str += u'\u03CB';
        } break;
        case u'\u03CF': {
            str += u'\u03D7';
        } break;
        case u'\u03D8': {
            str += u'\u03D9';
        } break;
        case u'\u03DA': {
            str += u'\u03DB';
        } break;
        case u'\u03DC': {
            str += u'\u03DD';
        } break;
        case u'\u03DE': {
            str += u'\u03DF';
        } break;
        case u'\u03E0': {
            str += u'\u03E1';
        } break;
        case u'\u03E2': {
            str += u'\u03E3';
        } break;
        case u'\u03E4': {
            str += u'\u03E5';
        } break;
        case u'\u03E6': {
            str += u'\u03E7';
        } break;
        case u'\u03E8': {
            str += u'\u03E9';
        } break;
        case u'\u03EA': {
            str += u'\u03EB';
        } break;
        case u'\u03EC': {
            str += u'\u03ED';
        } break;
        case u'\u03EE': {
            str += u'\u03EF';
        } break;
        case u'\u03F7': {
            str += u'\u03F8';
        } break;
        case u'\u03F9': {
            str += u'\u03F2';
        } break;
        case u'\u03FA': {
            str += u'\u03FB';
        } break;
        case u'\u03FD': {
            str += u'\u037B';
        } break;
        case u'\u03FE': {
            str += u'\u037C';
        } break;
        case u'\u03FF': {
            str += u'\u037D';
        } break;
        case u'\u0400': {
            str += u'\u0450';
        } break;
        case u'\u0401': {
            str += u'\u0451';
        } break;
        case u'\u0402': {
            str += u'\u0452';
        } break;
        case u'\u0403': {
            str += u'\u0453';
        } break;
        case u'\u0404': {
            str += u'\u0454';
        } break;
        case u'\u0405': {
            str += u'\u0455';
        } break;
        case u'\u0406': {
            str += u'\u0456';
        } break;
        case u'\u0407': {
            str += u'\u0457';
        } break;
        case u'\u0408': {
            str += u'\u0458';
        } break;
        case u'\u0409': {
            str += u'\u0459';
        } break;
        case u'\u040A': {
            str += u'\u045A';
        } break;
        case u'\u040B': {
            str += u'\u045B';
        } break;
        case u'\u040C': {
            str += u'\u045C';
        } break;
        case u'\u040D': {
            str += u'\u045D';
        } break;
        case u'\u040E': {
            str += u'\u045E';
        } break;
        case u'\u040F': {
            str += u'\u045F';
        } break;
        case u'\u0410': {
            str += u'\u0430';
        } break;
        case u'\u0411': {
            str += u'\u0431';
        } break;
        case u'\u0412': {
            str += u'\u0432';
        } break;
        case u'\u0413': {
            str += u'\u0433';
        } break;
        case u'\u0414': {
            str += u'\u0434';
        } break;
        case u'\u0415': {
            str += u'\u0435';
        } break;
        case u'\u0416': {
            str += u'\u0436';
        } break;
        case u'\u0417': {
            str += u'\u0437';
        } break;
        case u'\u0418': {
            str += u'\u0438';
        } break;
        case u'\u0419': {
            str += u'\u0439';
        } break;
        case u'\u041A': {
            str += u'\u043A';
        } break;
        case u'\u041B': {
            str += u'\u043B';
        } break;
        case u'\u041C': {
            str += u'\u043C';
        } break;
        case u'\u041D': {
            str += u'\u043D';
        } break;
        case u'\u041E': {
            str += u'\u043E';
        } break;
        case u'\u041F': {
            str += u'\u043F';
        } break;
        case u'\u0420': {
            str += u'\u0440';
        } break;
        case u'\u0421': {
            str += u'\u0441';
        } break;
        case u'\u0422': {
            str += u'\u0442';
        } break;
        case u'\u0423': {
            str += u'\u0443';
        } break;
        case u'\u0424': {
            str += u'\u0444';
        } break;
        case u'\u0425': {
            str += u'\u0445';
        } break;
        case u'\u0426': {
            str += u'\u0446';
        } break;
        case u'\u0427': {
            str += u'\u0447';
        } break;
        case u'\u0428': {
            str += u'\u0448';
        } break;
        case u'\u0429': {
            str += u'\u0449';
        } break;
        case u'\u042A': {
            str += u'\u044A';
        } break;
        case u'\u042B': {
            str += u'\u044B';
        } break;
        case u'\u042C': {
            str += u'\u044C';
        } break;
        case u'\u042D': {
            str += u'\u044D';
        } break;
        case u'\u042E': {
            str += u'\u044E';
        } break;
        case u'\u042F': {
            str += u'\u044F';
        } break;
        case u'\u0460': {
            str += u'\u0461';
        } break;
        case u'\u0462': {
            str += u'\u0463';
        } break;
        case u'\u0464': {
            str += u'\u0465';
        } break;
        case u'\u0466': {
            str += u'\u0467';
        } break;
        case u'\u0468': {
            str += u'\u0469';
        } break;
        case u'\u046A': {
            str += u'\u046B';
        } break;
        case u'\u046C': {
            str += u'\u046D';
        } break;
        case u'\u046E': {
            str += u'\u046F';
        } break;
        case u'\u0470': {
            str += u'\u0471';
        } break;
        case u'\u0472': {
            str += u'\u0473';
        } break;
        case u'\u0474': {
            str += u'\u0475';
        } break;
        case u'\u0476': {
            str += u'\u0477';
        } break;
        case u'\u0478': {
            str += u'\u0479';
        } break;
        case u'\u047A': {
            str += u'\u047B';
        } break;
        case u'\u047C': {
            str += u'\u047D';
        } break;
        case u'\u047E': {
            str += u'\u047F';
        } break;
        case u'\u0480': {
            str += u'\u0481';
        } break;
        case u'\u048A': {
            str += u'\u048B';
        } break;
        case u'\u048C': {
            str += u'\u048D';
        } break;
        case u'\u048E': {
            str += u'\u048F';
        } break;
        case u'\u0490': {
            str += u'\u0491';
        } break;
        case u'\u0492': {
            str += u'\u0493';
        } break;
        case u'\u0494': {
            str += u'\u0495';
        } break;
        case u'\u0496': {
            str += u'\u0497';
        } break;
        case u'\u0498': {
            str += u'\u0499';
        } break;
        case u'\u049A': {
            str += u'\u049B';
        } break;
        case u'\u049C': {
            str += u'\u049D';
        } break;
        case u'\u049E': {
            str += u'\u049F';
        } break;
        case u'\u04A0': {
            str += u'\u04A1';
        } break;
        case u'\u04A2': {
            str += u'\u04A3';
        } break;
        case u'\u04A4': {
            str += u'\u04A5';
        } break;
        case u'\u04A6': {
            str += u'\u04A7';
        } break;
        case u'\u04A8': {
            str += u'\u04A9';
        } break;
        case u'\u04AA': {
            str += u'\u04AB';
        } break;
        case u'\u04AC': {
            str += u'\u04AD';
        } break;
        case u'\u04AE': {
            str += u'\u04AF';
        } break;
        case u'\u04B0': {
            str += u'\u04B1';
        } break;
        case u'\u04B2': {
            str += u'\u04B3';
        } break;
        case u'\u04B4': {
            str += u'\u04B5';
        } break;
        case u'\u04B6': {
            str += u'\u04B7';
        } break;
        case u'\u04B8': {
            str += u'\u04B9';
        } break;
        case u'\u04BA': {
            str += u'\u04BB';
        } break;
        case u'\u04BC': {
            str += u'\u04BD';
        } break;
        case u'\u04BE': {
            str += u'\u04BF';
        } break;
        case u'\u04C0': {
            str += u'\u04CF';
        } break;
        case u'\u04C1': {
            str += u'\u04C2';
        } break;
        case u'\u04C3': {
            str += u'\u04C4';
        } break;
        case u'\u04C5': {
            str += u'\u04C6';
        } break;
        case u'\u04C7': {
            str += u'\u04C8';
        } break;
        case u'\u04C9': {
            str += u'\u04CA';
        } break;
        case u'\u04CB': {
            str += u'\u04CC';
        } break;
        case u'\u04CD': {
            str += u'\u04CE';
        } break;
        case u'\u04D0': {
            str += u'\u04D1';
        } break;
        case u'\u04D2': {
            str += u'\u04D3';
        } break;
        case u'\u04D4': {
            str += u'\u04D5';
        } break;
        case u'\u04D6': {
            str += u'\u04D7';
        } break;
        case u'\u04D8': {
            str += u'\u04D9';
        } break;
        case u'\u04DA': {
            str += u'\u04DB';
        } break;
        case u'\u04DC': {
            str += u'\u04DD';
        } break;
        case u'\u04DE': {
            str += u'\u04DF';
        } break;
        case u'\u04E0': {
            str += u'\u04E1';
        } break;
        case u'\u04E2': {
            str += u'\u04E3';
        } break;
        case u'\u04E4': {
            str += u'\u04E5';
        } break;
        case u'\u04E6': {
            str += u'\u04E7';
        } break;
        case u'\u04E8': {
            str += u'\u04E9';
        } break;
        case u'\u04EA': {
            str += u'\u04EB';
        } break;
        case u'\u04EC': {
            str += u'\u04ED';
        } break;
        case u'\u04EE': {
            str += u'\u04EF';
        } break;
        case u'\u04F0': {
            str += u'\u04F1';
        } break;
        case u'\u04F2': {
            str += u'\u04F3';
        } break;
        case u'\u04F4': {
            str += u'\u04F5';
        } break;
        case u'\u04F6': {
            str += u'\u04F7';
        } break;
        case u'\u04F8': {
            str += u'\u04F9';
        } break;
        case u'\u04FA': {
            str += u'\u04FB';
        } break;
        case u'\u04FC': {
            str += u'\u04FD';
        } break;
        case u'\u04FE': {
            str += u'\u04FF';
        } break;
        case u'\u0500': {
            str += u'\u0501';
        } break;
        case u'\u0502': {
            str += u'\u0503';
        } break;
        case u'\u0504': {
            str += u'\u0505';
        } break;
        case u'\u0506': {
            str += u'\u0507';
        } break;
        case u'\u0508': {
            str += u'\u0509';
        } break;
        case u'\u050A': {
            str += u'\u050B';
        } break;
        case u'\u050C': {
            str += u'\u050D';
        } break;
        case u'\u050E': {
            str += u'\u050F';
        } break;
        case u'\u0510': {
            str += u'\u0511';
        } break;
        case u'\u0512': {
            str += u'\u0513';
        } break;
        case u'\u0514': {
            str += u'\u0515';
        } break;
        case u'\u0516': {
            str += u'\u0517';
        } break;
        case u'\u0518': {
            str += u'\u0519';
        } break;
        case u'\u051A': {
            str += u'\u051B';
        } break;
        case u'\u051C': {
            str += u'\u051D';
        } break;
        case u'\u051E': {
            str += u'\u051F';
        } break;
        case u'\u0520': {
            str += u'\u0521';
        } break;
        case u'\u0522': {
            str += u'\u0523';
        } break;
        case u'\u0524': {
            str += u'\u0525';
        } break;
        case u'\u0526': {
            str += u'\u0527';
        } break;
        case u'\u0528': {
            str += u'\u0529';
        } break;
        case u'\u052A': {
            str += u'\u052B';
        } break;
        case u'\u052C': {
            str += u'\u052D';
        } break;
        case u'\u052E': {
            str += u'\u052F';
        } break;
        case u'\u0531': {
            str += u'\u0561';
        } break;
        case u'\u0532': {
            str += u'\u0562';
        } break;
        case u'\u0533': {
            str += u'\u0563';
        } break;
        case u'\u0534': {
            str += u'\u0564';
        } break;
        case u'\u0535': {
            str += u'\u0565';
        } break;
        case u'\u0536': {
            str += u'\u0566';
        } break;
        case u'\u0537': {
            str += u'\u0567';
        } break;
        case u'\u0538': {
            str += u'\u0568';
        } break;
        case u'\u0539': {
            str += u'\u0569';
        } break;
        case u'\u053A': {
            str += u'\u056A';
        } break;
        case u'\u053B': {
            str += u'\u056B';
        } break;
        case u'\u053C': {
            str += u'\u056C';
        } break;
        case u'\u053D': {
            str += u'\u056D';
        } break;
        case u'\u053E': {
            str += u'\u056E';
        } break;
        case u'\u053F': {
            str += u'\u056F';
        } break;
        case u'\u0540': {
            str += u'\u0570';
        } break;
        case u'\u0541': {
            str += u'\u0571';
        } break;
        case u'\u0542': {
            str += u'\u0572';
        } break;
        case u'\u0543': {
            str += u'\u0573';
        } break;
        case u'\u0544': {
            str += u'\u0574';
        } break;
        case u'\u0545': {
            str += u'\u0575';
        } break;
        case u'\u0546': {
            str += u'\u0576';
        } break;
        case u'\u0547': {
            str += u'\u0577';
        } break;
        case u'\u0548': {
            str += u'\u0578';
        } break;
        case u'\u0549': {
            str += u'\u0579';
        } break;
        case u'\u054A': {
            str += u'\u057A';
        } break;
        case u'\u054B': {
            str += u'\u057B';
        } break;
        case u'\u054C': {
            str += u'\u057C';
        } break;
        case u'\u054D': {
            str += u'\u057D';
        } break;
        case u'\u054E': {
            str += u'\u057E';
        } break;
        case u'\u054F': {
            str += u'\u057F';
        } break;
        case u'\u0550': {
            str += u'\u0580';
        } break;
        case u'\u0551': {
            str += u'\u0581';
        } break;
        case u'\u0552': {
            str += u'\u0582';
        } break;
        case u'\u0553': {
            str += u'\u0583';
        } break;
        case u'\u0554': {
            str += u'\u0584';
        } break;
        case u'\u0555': {
            str += u'\u0585';
        } break;
        case u'\u0556': {
            str += u'\u0586';
        } break;
        case u'\u10A0': {
            str += u'\u2D00';
        } break;
        case u'\u10A1': {
            str += u'\u2D01';
        } break;
        case u'\u10A2': {
            str += u'\u2D02';
        } break;
        case u'\u10A3': {
            str += u'\u2D03';
        } break;
        case u'\u10A4': {
            str += u'\u2D04';
        } break;
        case u'\u10A5': {
            str += u'\u2D05';
        } break;
        case u'\u10A6': {
            str += u'\u2D06';
        } break;
        case u'\u10A7': {
            str += u'\u2D07';
        } break;
        case u'\u10A8': {
            str += u'\u2D08';
        } break;
        case u'\u10A9': {
            str += u'\u2D09';
        } break;
        case u'\u10AA': {
            str += u'\u2D0A';
        } break;
        case u'\u10AB': {
            str += u'\u2D0B';
        } break;
        case u'\u10AC': {
            str += u'\u2D0C';
        } break;
        case u'\u10AD': {
            str += u'\u2D0D';
        } break;
        case u'\u10AE': {
            str += u'\u2D0E';
        } break;
        case u'\u10AF': {
            str += u'\u2D0F';
        } break;
        case u'\u10B0': {
            str += u'\u2D10';
        } break;
        case u'\u10B1': {
            str += u'\u2D11';
        } break;
        case u'\u10B2': {
            str += u'\u2D12';
        } break;
        case u'\u10B3': {
            str += u'\u2D13';
        } break;
        case u'\u10B4': {
            str += u'\u2D14';
        } break;
        case u'\u10B5': {
            str += u'\u2D15';
        } break;
        case u'\u10B6': {
            str += u'\u2D16';
        } break;
        case u'\u10B7': {
            str += u'\u2D17';
        } break;
        case u'\u10B8': {
            str += u'\u2D18';
        } break;
        case u'\u10B9': {
            str += u'\u2D19';
        } break;
        case u'\u10BA': {
            str += u'\u2D1A';
        } break;
        case u'\u10BB': {
            str += u'\u2D1B';
        } break;
        case u'\u10BC': {
            str += u'\u2D1C';
        } break;
        case u'\u10BD': {
            str += u'\u2D1D';
        } break;
        case u'\u10BE': {
            str += u'\u2D1E';
        } break;
        case u'\u10BF': {
            str += u'\u2D1F';
        } break;
        case u'\u10C0': {
            str += u'\u2D20';
        } break;
        case u'\u10C1': {
            str += u'\u2D21';
        } break;
        case u'\u10C2': {
            str += u'\u2D22';
        } break;
        case u'\u10C3': {
            str += u'\u2D23';
        } break;
        case u'\u10C4': {
            str += u'\u2D24';
        } break;
        case u'\u10C5': {
            str += u'\u2D25';
        } break;
        case u'\u10C7': {
            str += u'\u2D27';
        } break;
        case u'\u10CD': {
            str += u'\u2D2D';
        } break;
        case u'\u13A0': {
            str += u'\uAB70';
        } break;
        case u'\u13A1': {
            str += u'\uAB71';
        } break;
        case u'\u13A2': {
            str += u'\uAB72';
        } break;
        case u'\u13A3': {
            str += u'\uAB73';
        } break;
        case u'\u13A4': {
            str += u'\uAB74';
        } break;
        case u'\u13A5': {
            str += u'\uAB75';
        } break;
        case u'\u13A6': {
            str += u'\uAB76';
        } break;
        case u'\u13A7': {
            str += u'\uAB77';
        } break;
        case u'\u13A8': {
            str += u'\uAB78';
        } break;
        case u'\u13A9': {
            str += u'\uAB79';
        } break;
        case u'\u13AA': {
            str += u'\uAB7A';
        } break;
        case u'\u13AB': {
            str += u'\uAB7B';
        } break;
        case u'\u13AC': {
            str += u'\uAB7C';
        } break;
        case u'\u13AD': {
            str += u'\uAB7D';
        } break;
        case u'\u13AE': {
            str += u'\uAB7E';
        } break;
        case u'\u13AF': {
            str += u'\uAB7F';
        } break;
        case u'\u13B0': {
            str += u'\uAB80';
        } break;
        case u'\u13B1': {
            str += u'\uAB81';
        } break;
        case u'\u13B2': {
            str += u'\uAB82';
        } break;
        case u'\u13B3': {
            str += u'\uAB83';
        } break;
        case u'\u13B4': {
            str += u'\uAB84';
        } break;
        case u'\u13B5': {
            str += u'\uAB85';
        } break;
        case u'\u13B6': {
            str += u'\uAB86';
        } break;
        case u'\u13B7': {
            str += u'\uAB87';
        } break;
        case u'\u13B8': {
            str += u'\uAB88';
        } break;
        case u'\u13B9': {
            str += u'\uAB89';
        } break;
        case u'\u13BA': {
            str += u'\uAB8A';
        } break;
        case u'\u13BB': {
            str += u'\uAB8B';
        } break;
        case u'\u13BC': {
            str += u'\uAB8C';
        } break;
        case u'\u13BD': {
            str += u'\uAB8D';
        } break;
        case u'\u13BE': {
            str += u'\uAB8E';
        } break;
        case u'\u13BF': {
            str += u'\uAB8F';
        } break;
        case u'\u13C0': {
            str += u'\uAB90';
        } break;
        case u'\u13C1': {
            str += u'\uAB91';
        } break;
        case u'\u13C2': {
            str += u'\uAB92';
        } break;
        case u'\u13C3': {
            str += u'\uAB93';
        } break;
        case u'\u13C4': {
            str += u'\uAB94';
        } break;
        case u'\u13C5': {
            str += u'\uAB95';
        } break;
        case u'\u13C6': {
            str += u'\uAB96';
        } break;
        case u'\u13C7': {
            str += u'\uAB97';
        } break;
        case u'\u13C8': {
            str += u'\uAB98';
        } break;
        case u'\u13C9': {
            str += u'\uAB99';
        } break;
        case u'\u13CA': {
            str += u'\uAB9A';
        } break;
        case u'\u13CB': {
            str += u'\uAB9B';
        } break;
        case u'\u13CC': {
            str += u'\uAB9C';
        } break;
        case u'\u13CD': {
            str += u'\uAB9D';
        } break;
        case u'\u13CE': {
            str += u'\uAB9E';
        } break;
        case u'\u13CF': {
            str += u'\uAB9F';
        } break;
        case u'\u13D0': {
            str += u'\uABA0';
        } break;
        case u'\u13D1': {
            str += u'\uABA1';
        } break;
        case u'\u13D2': {
            str += u'\uABA2';
        } break;
        case u'\u13D3': {
            str += u'\uABA3';
        } break;
        case u'\u13D4': {
            str += u'\uABA4';
        } break;
        case u'\u13D5': {
            str += u'\uABA5';
        } break;
        case u'\u13D6': {
            str += u'\uABA6';
        } break;
        case u'\u13D7': {
            str += u'\uABA7';
        } break;
        case u'\u13D8': {
            str += u'\uABA8';
        } break;
        case u'\u13D9': {
            str += u'\uABA9';
        } break;
        case u'\u13DA': {
            str += u'\uABAA';
        } break;
        case u'\u13DB': {
            str += u'\uABAB';
        } break;
        case u'\u13DC': {
            str += u'\uABAC';
        } break;
        case u'\u13DD': {
            str += u'\uABAD';
        } break;
        case u'\u13DE': {
            str += u'\uABAE';
        } break;
        case u'\u13DF': {
            str += u'\uABAF';
        } break;
        case u'\u13E0': {
            str += u'\uABB0';
        } break;
        case u'\u13E1': {
            str += u'\uABB1';
        } break;
        case u'\u13E2': {
            str += u'\uABB2';
        } break;
        case u'\u13E3': {
            str += u'\uABB3';
        } break;
        case u'\u13E4': {
            str += u'\uABB4';
        } break;
        case u'\u13E5': {
            str += u'\uABB5';
        } break;
        case u'\u13E6': {
            str += u'\uABB6';
        } break;
        case u'\u13E7': {
            str += u'\uABB7';
        } break;
        case u'\u13E8': {
            str += u'\uABB8';
        } break;
        case u'\u13E9': {
            str += u'\uABB9';
        } break;
        case u'\u13EA': {
            str += u'\uABBA';
        } break;
        case u'\u13EB': {
            str += u'\uABBB';
        } break;
        case u'\u13EC': {
            str += u'\uABBC';
        } break;
        case u'\u13ED': {
            str += u'\uABBD';
        } break;
        case u'\u13EE': {
            str += u'\uABBE';
        } break;
        case u'\u13EF': {
            str += u'\uABBF';
        } break;
        case u'\u13F0': {
            str += u'\u13F8';
        } break;
        case u'\u13F1': {
            str += u'\u13F9';
        } break;
        case u'\u13F2': {
            str += u'\u13FA';
        } break;
        case u'\u13F3': {
            str += u'\u13FB';
        } break;
        case u'\u13F4': {
            str += u'\u13FC';
        } break;
        case u'\u13F5': {
            str += u'\u13FD';
        } break;
        case u'\u1C90': {
            str += u'\u10D0';
        } break;
        case u'\u1C91': {
            str += u'\u10D1';
        } break;
        case u'\u1C92': {
            str += u'\u10D2';
        } break;
        case u'\u1C93': {
            str += u'\u10D3';
        } break;
        case u'\u1C94': {
            str += u'\u10D4';
        } break;
        case u'\u1C95': {
            str += u'\u10D5';
        } break;
        case u'\u1C96': {
            str += u'\u10D6';
        } break;
        case u'\u1C97': {
            str += u'\u10D7';
        } break;
        case u'\u1C98': {
            str += u'\u10D8';
        } break;
        case u'\u1C99': {
            str += u'\u10D9';
        } break;
        case u'\u1C9A': {
            str += u'\u10DA';
        } break;
        case u'\u1C9B': {
            str += u'\u10DB';
        } break;
        case u'\u1C9C': {
            str += u'\u10DC';
        } break;
        case u'\u1C9D': {
            str += u'\u10DD';
        } break;
        case u'\u1C9E': {
            str += u'\u10DE';
        } break;
        case u'\u1C9F': {
            str += u'\u10DF';
        } break;
        case u'\u1CA0': {
            str += u'\u10E0';
        } break;
        case u'\u1CA1': {
            str += u'\u10E1';
        } break;
        case u'\u1CA2': {
            str += u'\u10E2';
        } break;
        case u'\u1CA3': {
            str += u'\u10E3';
        } break;
        case u'\u1CA4': {
            str += u'\u10E4';
        } break;
        case u'\u1CA5': {
            str += u'\u10E5';
        } break;
        case u'\u1CA6': {
            str += u'\u10E6';
        } break;
        case u'\u1CA7': {
            str += u'\u10E7';
        } break;
        case u'\u1CA8': {
            str += u'\u10E8';
        } break;
        case u'\u1CA9': {
            str += u'\u10E9';
        } break;
        case u'\u1CAA': {
            str += u'\u10EA';
        } break;
        case u'\u1CAB': {
            str += u'\u10EB';
        } break;
        case u'\u1CAC': {
            str += u'\u10EC';
        } break;
        case u'\u1CAD': {
            str += u'\u10ED';
        } break;
        case u'\u1CAE': {
            str += u'\u10EE';
        } break;
        case u'\u1CAF': {
            str += u'\u10EF';
        } break;
        case u'\u1CB0': {
            str += u'\u10F0';
        } break;
        case u'\u1CB1': {
            str += u'\u10F1';
        } break;
        case u'\u1CB2': {
            str += u'\u10F2';
        } break;
        case u'\u1CB3': {
            str += u'\u10F3';
        } break;
        case u'\u1CB4': {
            str += u'\u10F4';
        } break;
        case u'\u1CB5': {
            str += u'\u10F5';
        } break;
        case u'\u1CB6': {
            str += u'\u10F6';
        } break;
        case u'\u1CB7': {
            str += u'\u10F7';
        } break;
        case u'\u1CB8': {
            str += u'\u10F8';
        } break;
        case u'\u1CB9': {
            str += u'\u10F9';
        } break;
        case u'\u1CBA': {
            str += u'\u10FA';
        } break;
        case u'\u1CBD': {
            str += u'\u10FD';
        } break;
        case u'\u1CBE': {
            str += u'\u10FE';
        } break;
        case u'\u1CBF': {
            str += u'\u10FF';
        } break;
        case u'\u1E00': {
            str += u'\u1E01';
        } break;
        case u'\u1E02': {
            str += u'\u1E03';
        } break;
        case u'\u1E04': {
            str += u'\u1E05';
        } break;
        case u'\u1E06': {
            str += u'\u1E07';
        } break;
        case u'\u1E08': {
            str += u'\u1E09';
        } break;
        case u'\u1E0A': {
            str += u'\u1E0B';
        } break;
        case u'\u1E0C': {
            str += u'\u1E0D';
        } break;
        case u'\u1E0E': {
            str += u'\u1E0F';
        } break;
        case u'\u1E10': {
            str += u'\u1E11';
        } break;
        case u'\u1E12': {
            str += u'\u1E13';
        } break;
        case u'\u1E14': {
            str += u'\u1E15';
        } break;
        case u'\u1E16': {
            str += u'\u1E17';
        } break;
        case u'\u1E18': {
            str += u'\u1E19';
        } break;
        case u'\u1E1A': {
            str += u'\u1E1B';
        } break;
        case u'\u1E1C': {
            str += u'\u1E1D';
        } break;
        case u'\u1E1E': {
            str += u'\u1E1F';
        } break;
        case u'\u1E20': {
            str += u'\u1E21';
        } break;
        case u'\u1E22': {
            str += u'\u1E23';
        } break;
        case u'\u1E24': {
            str += u'\u1E25';
        } break;
        case u'\u1E26': {
            str += u'\u1E27';
        } break;
        case u'\u1E28': {
            str += u'\u1E29';
        } break;
        case u'\u1E2A': {
            str += u'\u1E2B';
        } break;
        case u'\u1E2C': {
            str += u'\u1E2D';
        } break;
        case u'\u1E2E': {
            str += u'\u1E2F';
        } break;
        case u'\u1E30': {
            str += u'\u1E31';
        } break;
        case u'\u1E32': {
            str += u'\u1E33';
        } break;
        case u'\u1E34': {
            str += u'\u1E35';
        } break;
        case u'\u1E36': {
            str += u'\u1E37';
        } break;
        case u'\u1E38': {
            str += u'\u1E39';
        } break;
        case u'\u1E3A': {
            str += u'\u1E3B';
        } break;
        case u'\u1E3C': {
            str += u'\u1E3D';
        } break;
        case u'\u1E3E': {
            str += u'\u1E3F';
        } break;
        case u'\u1E40': {
            str += u'\u1E41';
        } break;
        case u'\u1E42': {
            str += u'\u1E43';
        } break;
        case u'\u1E44': {
            str += u'\u1E45';
        } break;
        case u'\u1E46': {
            str += u'\u1E47';
        } break;
        case u'\u1E48': {
            str += u'\u1E49';
        } break;
        case u'\u1E4A': {
            str += u'\u1E4B';
        } break;
        case u'\u1E4C': {
            str += u'\u1E4D';
        } break;
        case u'\u1E4E': {
            str += u'\u1E4F';
        } break;
        case u'\u1E50': {
            str += u'\u1E51';
        } break;
        case u'\u1E52': {
            str += u'\u1E53';
        } break;
        case u'\u1E54': {
            str += u'\u1E55';
        } break;
        case u'\u1E56': {
            str += u'\u1E57';
        } break;
        case u'\u1E58': {
            str += u'\u1E59';
        } break;
        case u'\u1E5A': {
            str += u'\u1E5B';
        } break;
        case u'\u1E5C': {
            str += u'\u1E5D';
        } break;
        case u'\u1E5E': {
            str += u'\u1E5F';
        } break;
        case u'\u1E60': {
            str += u'\u1E61';
        } break;
        case u'\u1E62': {
            str += u'\u1E63';
        } break;
        case u'\u1E64': {
            str += u'\u1E65';
        } break;
        case u'\u1E66': {
            str += u'\u1E67';
        } break;
        case u'\u1E68': {
            str += u'\u1E69';
        } break;
        case u'\u1E6A': {
            str += u'\u1E6B';
        } break;
        case u'\u1E6C': {
            str += u'\u1E6D';
        } break;
        case u'\u1E6E': {
            str += u'\u1E6F';
        } break;
        case u'\u1E70': {
            str += u'\u1E71';
        } break;
        case u'\u1E72': {
            str += u'\u1E73';
        } break;
        case u'\u1E74': {
            str += u'\u1E75';
        } break;
        case u'\u1E76': {
            str += u'\u1E77';
        } break;
        case u'\u1E78': {
            str += u'\u1E79';
        } break;
        case u'\u1E7A': {
            str += u'\u1E7B';
        } break;
        case u'\u1E7C': {
            str += u'\u1E7D';
        } break;
        case u'\u1E7E': {
            str += u'\u1E7F';
        } break;
        case u'\u1E80': {
            str += u'\u1E81';
        } break;
        case u'\u1E82': {
            str += u'\u1E83';
        } break;
        case u'\u1E84': {
            str += u'\u1E85';
        } break;
        case u'\u1E86': {
            str += u'\u1E87';
        } break;
        case u'\u1E88': {
            str += u'\u1E89';
        } break;
        case u'\u1E8A': {
            str += u'\u1E8B';
        } break;
        case u'\u1E8C': {
            str += u'\u1E8D';
        } break;
        case u'\u1E8E': {
            str += u'\u1E8F';
        } break;
        case u'\u1E90': {
            str += u'\u1E91';
        } break;
        case u'\u1E92': {
            str += u'\u1E93';
        } break;
        case u'\u1E94': {
            str += u'\u1E95';
        } break;
        case u'\u1E9E': {
            str += u'\u00DF';
        } break;
        case u'\u1EA0': {
            str += u'\u1EA1';
        } break;
        case u'\u1EA2': {
            str += u'\u1EA3';
        } break;
        case u'\u1EA4': {
            str += u'\u1EA5';
        } break;
        case u'\u1EA6': {
            str += u'\u1EA7';
        } break;
        case u'\u1EA8': {
            str += u'\u1EA9';
        } break;
        case u'\u1EAA': {
            str += u'\u1EAB';
        } break;
        case u'\u1EAC': {
            str += u'\u1EAD';
        } break;
        case u'\u1EAE': {
            str += u'\u1EAF';
        } break;
        case u'\u1EB0': {
            str += u'\u1EB1';
        } break;
        case u'\u1EB2': {
            str += u'\u1EB3';
        } break;
        case u'\u1EB4': {
            str += u'\u1EB5';
        } break;
        case u'\u1EB6': {
            str += u'\u1EB7';
        } break;
        case u'\u1EB8': {
            str += u'\u1EB9';
        } break;
        case u'\u1EBA': {
            str += u'\u1EBB';
        } break;
        case u'\u1EBC': {
            str += u'\u1EBD';
        } break;
        case u'\u1EBE': {
            str += u'\u1EBF';
        } break;
        case u'\u1EC0': {
            str += u'\u1EC1';
        } break;
        case u'\u1EC2': {
            str += u'\u1EC3';
        } break;
        case u'\u1EC4': {
            str += u'\u1EC5';
        } break;
        case u'\u1EC6': {
            str += u'\u1EC7';
        } break;
        case u'\u1EC8': {
            str += u'\u1EC9';
        } break;
        case u'\u1ECA': {
            str += u'\u1ECB';
        } break;
        case u'\u1ECC': {
            str += u'\u1ECD';
        } break;
        case u'\u1ECE': {
            str += u'\u1ECF';
        } break;
        case u'\u1ED0': {
            str += u'\u1ED1';
        } break;
        case u'\u1ED2': {
            str += u'\u1ED3';
        } break;
        case u'\u1ED4': {
            str += u'\u1ED5';
        } break;
        case u'\u1ED6': {
            str += u'\u1ED7';
        } break;
        case u'\u1ED8': {
            str += u'\u1ED9';
        } break;
        case u'\u1EDA': {
            str += u'\u1EDB';
        } break;
        case u'\u1EDC': {
            str += u'\u1EDD';
        } break;
        case u'\u1EDE': {
            str += u'\u1EDF';
        } break;
        case u'\u1EE0': {
            str += u'\u1EE1';
        } break;
        case u'\u1EE2': {
            str += u'\u1EE3';
        } break;
        case u'\u1EE4': {
            str += u'\u1EE5';
        } break;
        case u'\u1EE6': {
            str += u'\u1EE7';
        } break;
        case u'\u1EE8': {
            str += u'\u1EE9';
        } break;
        case u'\u1EEA': {
            str += u'\u1EEB';
        } break;
        case u'\u1EEC': {
            str += u'\u1EED';
        } break;
        case u'\u1EEE': {
            str += u'\u1EEF';
        } break;
        case u'\u1EF0': {
            str += u'\u1EF1';
        } break;
        case u'\u1EF2': {
            str += u'\u1EF3';
        } break;
        case u'\u1EF4': {
            str += u'\u1EF5';
        } break;
        case u'\u1EF6': {
            str += u'\u1EF7';
        } break;
        case u'\u1EF8': {
            str += u'\u1EF9';
        } break;
        case u'\u1EFA': {
            str += u'\u1EFB';
        } break;
        case u'\u1EFC': {
            str += u'\u1EFD';
        } break;
        case u'\u1EFE': {
            str += u'\u1EFF';
        } break;
        case u'\u1F08': {
            str += u'\u1F00';
        } break;
        case u'\u1F09': {
            str += u'\u1F01';
        } break;
        case u'\u1F0A': {
            str += u'\u1F02';
        } break;
        case u'\u1F0B': {
            str += u'\u1F03';
        } break;
        case u'\u1F0C': {
            str += u'\u1F04';
        } break;
        case u'\u1F0D': {
            str += u'\u1F05';
        } break;
        case u'\u1F0E': {
            str += u'\u1F06';
        } break;
        case u'\u1F0F': {
            str += u'\u1F07';
        } break;
        case u'\u1F18': {
            str += u'\u1F10';
        } break;
        case u'\u1F19': {
            str += u'\u1F11';
        } break;
        case u'\u1F1A': {
            str += u'\u1F12';
        } break;
        case u'\u1F1B': {
            str += u'\u1F13';
        } break;
        case u'\u1F1C': {
            str += u'\u1F14';
        } break;
        case u'\u1F1D': {
            str += u'\u1F15';
        } break;
        case u'\u1F28': {
            str += u'\u1F20';
        } break;
        case u'\u1F29': {
            str += u'\u1F21';
        } break;
        case u'\u1F2A': {
            str += u'\u1F22';
        } break;
        case u'\u1F2B': {
            str += u'\u1F23';
        } break;
        case u'\u1F2C': {
            str += u'\u1F24';
        } break;
        case u'\u1F2D': {
            str += u'\u1F25';
        } break;
        case u'\u1F2E': {
            str += u'\u1F26';
        } break;
        case u'\u1F2F': {
            str += u'\u1F27';
        } break;
        case u'\u1F38': {
            str += u'\u1F30';
        } break;
        case u'\u1F39': {
            str += u'\u1F31';
        } break;
        case u'\u1F3A': {
            str += u'\u1F32';
        } break;
        case u'\u1F3B': {
            str += u'\u1F33';
        } break;
        case u'\u1F3C': {
            str += u'\u1F34';
        } break;
        case u'\u1F3D': {
            str += u'\u1F35';
        } break;
        case u'\u1F3E': {
            str += u'\u1F36';
        } break;
        case u'\u1F3F': {
            str += u'\u1F37';
        } break;
        case u'\u1F48': {
            str += u'\u1F40';
        } break;
        case u'\u1F49': {
            str += u'\u1F41';
        } break;
        case u'\u1F4A': {
            str += u'\u1F42';
        } break;
        case u'\u1F4B': {
            str += u'\u1F43';
        } break;
        case u'\u1F4C': {
            str += u'\u1F44';
        } break;
        case u'\u1F4D': {
            str += u'\u1F45';
        } break;
        case u'\u1F59': {
            str += u'\u1F51';
        } break;
        case u'\u1F5B': {
            str += u'\u1F53';
        } break;
        case u'\u1F5D': {
            str += u'\u1F55';
        } break;
        case u'\u1F5F': {
            str += u'\u1F57';
        } break;
        case u'\u1F68': {
            str += u'\u1F60';
        } break;
        case u'\u1F69': {
            str += u'\u1F61';
        } break;
        case u'\u1F6A': {
            str += u'\u1F62';
        } break;
        case u'\u1F6B': {
            str += u'\u1F63';
        } break;
        case u'\u1F6C': {
            str += u'\u1F64';
        } break;
        case u'\u1F6D': {
            str += u'\u1F65';
        } break;
        case u'\u1F6E': {
            str += u'\u1F66';
        } break;
        case u'\u1F6F': {
            str += u'\u1F67';
        } break;
        case u'\u1F88': {
            str += u'\u1F80';
        } break;
        case u'\u1F89': {
            str += u'\u1F81';
        } break;
        case u'\u1F8A': {
            str += u'\u1F82';
        } break;
        case u'\u1F8B': {
            str += u'\u1F83';
        } break;
        case u'\u1F8C': {
            str += u'\u1F84';
        } break;
        case u'\u1F8D': {
            str += u'\u1F85';
        } break;
        case u'\u1F8E': {
            str += u'\u1F86';
        } break;
        case u'\u1F8F': {
            str += u'\u1F87';
        } break;
        case u'\u1F98': {
            str += u'\u1F90';
        } break;
        case u'\u1F99': {
            str += u'\u1F91';
        } break;
        case u'\u1F9A': {
            str += u'\u1F92';
        } break;
        case u'\u1F9B': {
            str += u'\u1F93';
        } break;
        case u'\u1F9C': {
            str += u'\u1F94';
        } break;
        case u'\u1F9D': {
            str += u'\u1F95';
        } break;
        case u'\u1F9E': {
            str += u'\u1F96';
        } break;
        case u'\u1F9F': {
            str += u'\u1F97';
        } break;
        case u'\u1FA8': {
            str += u'\u1FA0';
        } break;
        case u'\u1FA9': {
            str += u'\u1FA1';
        } break;
        case u'\u1FAA': {
            str += u'\u1FA2';
        } break;
        case u'\u1FAB': {
            str += u'\u1FA3';
        } break;
        case u'\u1FAC': {
            str += u'\u1FA4';
        } break;
        case u'\u1FAD': {
            str += u'\u1FA5';
        } break;
        case u'\u1FAE': {
            str += u'\u1FA6';
        } break;
        case u'\u1FAF': {
            str += u'\u1FA7';
        } break;
        case u'\u1FB8': {
            str += u'\u1FB0';
        } break;
        case u'\u1FB9': {
            str += u'\u1FB1';
        } break;
        case u'\u1FBA': {
            str += u'\u1F70';
        } break;
        case u'\u1FBB': {
            str += u'\u1F71';
        } break;
        case u'\u1FBC': {
            str += u'\u1FB3';
        } break;
        case u'\u1FC8': {
            str += u'\u1F72';
        } break;
        case u'\u1FC9': {
            str += u'\u1F73';
        } break;
        case u'\u1FCA': {
            str += u'\u1F74';
        } break;
        case u'\u1FCB': {
            str += u'\u1F75';
        } break;
        case u'\u1FCC': {
            str += u'\u1FC3';
        } break;
        case u'\u1FD8': {
            str += u'\u1FD0';
        } break;
        case u'\u1FD9': {
            str += u'\u1FD1';
        } break;
        case u'\u1FDA': {
            str += u'\u1F76';
        } break;
        case u'\u1FDB': {
            str += u'\u1F77';
        } break;
        case u'\u1FE8': {
            str += u'\u1FE0';
        } break;
        case u'\u1FE9': {
            str += u'\u1FE1';
        } break;
        case u'\u1FEA': {
            str += u'\u1F7A';
        } break;
        case u'\u1FEB': {
            str += u'\u1F7B';
        } break;
        case u'\u1FEC': {
            str += u'\u1FE5';
        } break;
        case u'\u1FF8': {
            str += u'\u1F78';
        } break;
        case u'\u1FF9': {
            str += u'\u1F79';
        } break;
        case u'\u1FFA': {
            str += u'\u1F7C';
        } break;
        case u'\u1FFB': {
            str += u'\u1F7D';
        } break;
        case u'\u1FFC': {
            str += u'\u1FF3';
        } break;
        case u'\u2132': {
            str += u'\u214E';
        } break;
        case u'\u2160': {
            str += u'\u2170';
        } break;
        case u'\u2161': {
            str += u'\u2171';
        } break;
        case u'\u2162': {
            str += u'\u2172';
        } break;
        case u'\u2163': {
            str += u'\u2173';
        } break;
        case u'\u2164': {
            str += u'\u2174';
        } break;
        case u'\u2165': {
            str += u'\u2175';
        } break;
        case u'\u2166': {
            str += u'\u2176';
        } break;
        case u'\u2167': {
            str += u'\u2177';
        } break;
        case u'\u2168': {
            str += u'\u2178';
        } break;
        case u'\u2169': {
            str += u'\u2179';
        } break;
        case u'\u216A': {
            str += u'\u217A';
        } break;
        case u'\u216B': {
            str += u'\u217B';
        } break;
        case u'\u216C': {
            str += u'\u217C';
        } break;
        case u'\u216D': {
            str += u'\u217D';
        } break;
        case u'\u216E': {
            str += u'\u217E';
        } break;
        case u'\u216F': {
            str += u'\u217F';
        } break;
        case u'\u2183': {
            str += u'\u2184';
        } break;
        case u'\u24B6': {
            str += u'\u24D0';
        } break;
        case u'\u24B7': {
            str += u'\u24D1';
        } break;
        case u'\u24B8': {
            str += u'\u24D2';
        } break;
        case u'\u24B9': {
            str += u'\u24D3';
        } break;
        case u'\u24BA': {
            str += u'\u24D4';
        } break;
        case u'\u24BB': {
            str += u'\u24D5';
        } break;
        case u'\u24BC': {
            str += u'\u24D6';
        } break;
        case u'\u24BD': {
            str += u'\u24D7';
        } break;
        case u'\u24BE': {
            str += u'\u24D8';
        } break;
        case u'\u24BF': {
            str += u'\u24D9';
        } break;
        case u'\u24C0': {
            str += u'\u24DA';
        } break;
        case u'\u24C1': {
            str += u'\u24DB';
        } break;
        case u'\u24C2': {
            str += u'\u24DC';
        } break;
        case u'\u24C3': {
            str += u'\u24DD';
        } break;
        case u'\u24C4': {
            str += u'\u24DE';
        } break;
        case u'\u24C5': {
            str += u'\u24DF';
        } break;
        case u'\u24C6': {
            str += u'\u24E0';
        } break;
        case u'\u24C7': {
            str += u'\u24E1';
        } break;
        case u'\u24C8': {
            str += u'\u24E2';
        } break;
        case u'\u24C9': {
            str += u'\u24E3';
        } break;
        case u'\u24CA': {
            str += u'\u24E4';
        } break;
        case u'\u24CB': {
            str += u'\u24E5';
        } break;
        case u'\u24CC': {
            str += u'\u24E6';
        } break;
        case u'\u24CD': {
            str += u'\u24E7';
        } break;
        case u'\u24CE': {
            str += u'\u24E8';
        } break;
        case u'\u24CF': {
            str += u'\u24E9';
        } break;
        case u'\u2C00': {
            str += u'\u2C30';
        } break;
        case u'\u2C01': {
            str += u'\u2C31';
        } break;
        case u'\u2C02': {
            str += u'\u2C32';
        } break;
        case u'\u2C03': {
            str += u'\u2C33';
        } break;
        case u'\u2C04': {
            str += u'\u2C34';
        } break;
        case u'\u2C05': {
            str += u'\u2C35';
        } break;
        case u'\u2C06': {
            str += u'\u2C36';
        } break;
        case u'\u2C07': {
            str += u'\u2C37';
        } break;
        case u'\u2C08': {
            str += u'\u2C38';
        } break;
        case u'\u2C09': {
            str += u'\u2C39';
        } break;
        case u'\u2C0A': {
            str += u'\u2C3A';
        } break;
        case u'\u2C0B': {
            str += u'\u2C3B';
        } break;
        case u'\u2C0C': {
            str += u'\u2C3C';
        } break;
        case u'\u2C0D': {
            str += u'\u2C3D';
        } break;
        case u'\u2C0E': {
            str += u'\u2C3E';
        } break;
        case u'\u2C0F': {
            str += u'\u2C3F';
        } break;
        case u'\u2C10': {
            str += u'\u2C40';
        } break;
        case u'\u2C11': {
            str += u'\u2C41';
        } break;
        case u'\u2C12': {
            str += u'\u2C42';
        } break;
        case u'\u2C13': {
            str += u'\u2C43';
        } break;
        case u'\u2C14': {
            str += u'\u2C44';
        } break;
        case u'\u2C15': {
            str += u'\u2C45';
        } break;
        case u'\u2C16': {
            str += u'\u2C46';
        } break;
        case u'\u2C17': {
            str += u'\u2C47';
        } break;
        case u'\u2C18': {
            str += u'\u2C48';
        } break;
        case u'\u2C19': {
            str += u'\u2C49';
        } break;
        case u'\u2C1A': {
            str += u'\u2C4A';
        } break;
        case u'\u2C1B': {
            str += u'\u2C4B';
        } break;
        case u'\u2C1C': {
            str += u'\u2C4C';
        } break;
        case u'\u2C1D': {
            str += u'\u2C4D';
        } break;
        case u'\u2C1E': {
            str += u'\u2C4E';
        } break;
        case u'\u2C1F': {
            str += u'\u2C4F';
        } break;
        case u'\u2C20': {
            str += u'\u2C50';
        } break;
        case u'\u2C21': {
            str += u'\u2C51';
        } break;
        case u'\u2C22': {
            str += u'\u2C52';
        } break;
        case u'\u2C23': {
            str += u'\u2C53';
        } break;
        case u'\u2C24': {
            str += u'\u2C54';
        } break;
        case u'\u2C25': {
            str += u'\u2C55';
        } break;
        case u'\u2C26': {
            str += u'\u2C56';
        } break;
        case u'\u2C27': {
            str += u'\u2C57';
        } break;
        case u'\u2C28': {
            str += u'\u2C58';
        } break;
        case u'\u2C29': {
            str += u'\u2C59';
        } break;
        case u'\u2C2A': {
            str += u'\u2C5A';
        } break;
        case u'\u2C2B': {
            str += u'\u2C5B';
        } break;
        case u'\u2C2C': {
            str += u'\u2C5C';
        } break;
        case u'\u2C2D': {
            str += u'\u2C5D';
        } break;
        case u'\u2C2E': {
            str += u'\u2C5E';
        } break;
        case u'\u2C2F': {
            str += u'\u2C5F';
        } break;
        case u'\u2C60': {
            str += u'\u2C61';
        } break;
        case u'\u2C62': {
            str += u'\u026B';
        } break;
        case u'\u2C63': {
            str += u'\u1D7D';
        } break;
        case u'\u2C64': {
            str += u'\u027D';
        } break;
        case u'\u2C67': {
            str += u'\u2C68';
        } break;
        case u'\u2C69': {
            str += u'\u2C6A';
        } break;
        case u'\u2C6B': {
            str += u'\u2C6C';
        } break;
        case u'\u2C6D': {
            str += u'\u0251';
        } break;
        case u'\u2C6E': {
            str += u'\u0271';
        } break;
        case u'\u2C6F': {
            str += u'\u0250';
        } break;
        case u'\u2C70': {
            str += u'\u0252';
        } break;
        case u'\u2C72': {
            str += u'\u2C73';
        } break;
        case u'\u2C75': {
            str += u'\u2C76';
        } break;
        case u'\u2C7E': {
            str += u'\u023F';
        } break;
        case u'\u2C7F': {
            str += u'\u0240';
        } break;
        case u'\u2C80': {
            str += u'\u2C81';
        } break;
        case u'\u2C82': {
            str += u'\u2C83';
        } break;
        case u'\u2C84': {
            str += u'\u2C85';
        } break;
        case u'\u2C86': {
            str += u'\u2C87';
        } break;
        case u'\u2C88': {
            str += u'\u2C89';
        } break;
        case u'\u2C8A': {
            str += u'\u2C8B';
        } break;
        case u'\u2C8C': {
            str += u'\u2C8D';
        } break;
        case u'\u2C8E': {
            str += u'\u2C8F';
        } break;
        case u'\u2C90': {
            str += u'\u2C91';
        } break;
        case u'\u2C92': {
            str += u'\u2C93';
        } break;
        case u'\u2C94': {
            str += u'\u2C95';
        } break;
        case u'\u2C96': {
            str += u'\u2C97';
        } break;
        case u'\u2C98': {
            str += u'\u2C99';
        } break;
        case u'\u2C9A': {
            str += u'\u2C9B';
        } break;
        case u'\u2C9C': {
            str += u'\u2C9D';
        } break;
        case u'\u2C9E': {
            str += u'\u2C9F';
        } break;
        case u'\u2CA0': {
            str += u'\u2CA1';
        } break;
        case u'\u2CA2': {
            str += u'\u2CA3';
        } break;
        case u'\u2CA4': {
            str += u'\u2CA5';
        } break;
        case u'\u2CA6': {
            str += u'\u2CA7';
        } break;
        case u'\u2CA8': {
            str += u'\u2CA9';
        } break;
        case u'\u2CAA': {
            str += u'\u2CAB';
        } break;
        case u'\u2CAC': {
            str += u'\u2CAD';
        } break;
        case u'\u2CAE': {
            str += u'\u2CAF';
        } break;
        case u'\u2CB0': {
            str += u'\u2CB1';
        } break;
        case u'\u2CB2': {
            str += u'\u2CB3';
        } break;
        case u'\u2CB4': {
            str += u'\u2CB5';
        } break;
        case u'\u2CB6': {
            str += u'\u2CB7';
        } break;
        case u'\u2CB8': {
            str += u'\u2CB9';
        } break;
        case u'\u2CBA': {
            str += u'\u2CBB';
        } break;
        case u'\u2CBC': {
            str += u'\u2CBD';
        } break;
        case u'\u2CBE': {
            str += u'\u2CBF';
        } break;
        case u'\u2CC0': {
            str += u'\u2CC1';
        } break;
        case u'\u2CC2': {
            str += u'\u2CC3';
        } break;
        case u'\u2CC4': {
            str += u'\u2CC5';
        } break;
        case u'\u2CC6': {
            str += u'\u2CC7';
        } break;
        case u'\u2CC8': {
            str += u'\u2CC9';
        } break;
        case u'\u2CCA': {
            str += u'\u2CCB';
        } break;
        case u'\u2CCC': {
            str += u'\u2CCD';
        } break;
        case u'\u2CCE': {
            str += u'\u2CCF';
        } break;
        case u'\u2CD0': {
            str += u'\u2CD1';
        } break;
        case u'\u2CD2': {
            str += u'\u2CD3';
        } break;
        case u'\u2CD4': {
            str += u'\u2CD5';
        } break;
        case u'\u2CD6': {
            str += u'\u2CD7';
        } break;
        case u'\u2CD8': {
            str += u'\u2CD9';
        } break;
        case u'\u2CDA': {
            str += u'\u2CDB';
        } break;
        case u'\u2CDC': {
            str += u'\u2CDD';
        } break;
        case u'\u2CDE': {
            str += u'\u2CDF';
        } break;
        case u'\u2CE0': {
            str += u'\u2CE1';
        } break;
        case u'\u2CE2': {
            str += u'\u2CE3';
        } break;
        case u'\u2CEB': {
            str += u'\u2CEC';
        } break;
        case u'\u2CED': {
            str += u'\u2CEE';
        } break;
        case u'\u2CF2': {
            str += u'\u2CF3';
        } break;
        case u'\uA640': {
            str += u'\uA641';
        } break;
        case u'\uA642': {
            str += u'\uA643';
        } break;
        case u'\uA644': {
            str += u'\uA645';
        } break;
        case u'\uA646': {
            str += u'\uA647';
        } break;
        case u'\uA648': {
            str += u'\uA649';
        } break;
        case u'\uA64A': {
            str += u'\uA64B';
        } break;
        case u'\uA64C': {
            str += u'\uA64D';
        } break;
        case u'\uA64E': {
            str += u'\uA64F';
        } break;
        case u'\uA650': {
            str += u'\uA651';
        } break;
        case u'\uA652': {
            str += u'\uA653';
        } break;
        case u'\uA654': {
            str += u'\uA655';
        } break;
        case u'\uA656': {
            str += u'\uA657';
        } break;
        case u'\uA658': {
            str += u'\uA659';
        } break;
        case u'\uA65A': {
            str += u'\uA65B';
        } break;
        case u'\uA65C': {
            str += u'\uA65D';
        } break;
        case u'\uA65E': {
            str += u'\uA65F';
        } break;
        case u'\uA660': {
            str += u'\uA661';
        } break;
        case u'\uA662': {
            str += u'\uA663';
        } break;
        case u'\uA664': {
            str += u'\uA665';
        } break;
        case u'\uA666': {
            str += u'\uA667';
        } break;
        case u'\uA668': {
            str += u'\uA669';
        } break;
        case u'\uA66A': {
            str += u'\uA66B';
        } break;
        case u'\uA66C': {
            str += u'\uA66D';
        } break;
        case u'\uA680': {
            str += u'\uA681';
        } break;
        case u'\uA682': {
            str += u'\uA683';
        } break;
        case u'\uA684': {
            str += u'\uA685';
        } break;
        case u'\uA686': {
            str += u'\uA687';
        } break;
        case u'\uA688': {
            str += u'\uA689';
        } break;
        case u'\uA68A': {
            str += u'\uA68B';
        } break;
        case u'\uA68C': {
            str += u'\uA68D';
        } break;
        case u'\uA68E': {
            str += u'\uA68F';
        } break;
        case u'\uA690': {
            str += u'\uA691';
        } break;
        case u'\uA692': {
            str += u'\uA693';
        } break;
        case u'\uA694': {
            str += u'\uA695';
        } break;
        case u'\uA696': {
            str += u'\uA697';
        } break;
        case u'\uA698': {
            str += u'\uA699';
        } break;
        case u'\uA69A': {
            str += u'\uA69B';
        } break;
        case u'\uA722': {
            str += u'\uA723';
        } break;
        case u'\uA724': {
            str += u'\uA725';
        } break;
        case u'\uA726': {
            str += u'\uA727';
        } break;
        case u'\uA728': {
            str += u'\uA729';
        } break;
        case u'\uA72A': {
            str += u'\uA72B';
        } break;
        case u'\uA72C': {
            str += u'\uA72D';
        } break;
        case u'\uA72E': {
            str += u'\uA72F';
        } break;
        case u'\uA732': {
            str += u'\uA733';
        } break;
        case u'\uA734': {
            str += u'\uA735';
        } break;
        case u'\uA736': {
            str += u'\uA737';
        } break;
        case u'\uA738': {
            str += u'\uA739';
        } break;
        case u'\uA73A': {
            str += u'\uA73B';
        } break;
        case u'\uA73C': {
            str += u'\uA73D';
        } break;
        case u'\uA73E': {
            str += u'\uA73F';
        } break;
        case u'\uA740': {
            str += u'\uA741';
        } break;
        case u'\uA742': {
            str += u'\uA743';
        } break;
        case u'\uA744': {
            str += u'\uA745';
        } break;
        case u'\uA746': {
            str += u'\uA747';
        } break;
        case u'\uA748': {
            str += u'\uA749';
        } break;
        case u'\uA74A': {
            str += u'\uA74B';
        } break;
        case u'\uA74C': {
            str += u'\uA74D';
        } break;
        case u'\uA74E': {
            str += u'\uA74F';
        } break;
        case u'\uA750': {
            str += u'\uA751';
        } break;
        case u'\uA752': {
            str += u'\uA753';
        } break;
        case u'\uA754': {
            str += u'\uA755';
        } break;
        case u'\uA756': {
            str += u'\uA757';
        } break;
        case u'\uA758': {
            str += u'\uA759';
        } break;
        case u'\uA75A': {
            str += u'\uA75B';
        } break;
        case u'\uA75C': {
            str += u'\uA75D';
        } break;
        case u'\uA75E': {
            str += u'\uA75F';
        } break;
        case u'\uA760': {
            str += u'\uA761';
        } break;
        case u'\uA762': {
            str += u'\uA763';
        } break;
        case u'\uA764': {
            str += u'\uA765';
        } break;
        case u'\uA766': {
            str += u'\uA767';
        } break;
        case u'\uA768': {
            str += u'\uA769';
        } break;
        case u'\uA76A': {
            str += u'\uA76B';
        } break;
        case u'\uA76C': {
            str += u'\uA76D';
        } break;
        case u'\uA76E': {
            str += u'\uA76F';
        } break;
        case u'\uA779': {
            str += u'\uA77A';
        } break;
        case u'\uA77B': {
            str += u'\uA77C';
        } break;
        case u'\uA77D': {
            str += u'\u1D79';
        } break;
        case u'\uA77E': {
            str += u'\uA77F';
        } break;
        case u'\uA780': {
            str += u'\uA781';
        } break;
        case u'\uA782': {
            str += u'\uA783';
        } break;
        case u'\uA784': {
            str += u'\uA785';
        } break;
        case u'\uA786': {
            str += u'\uA787';
        } break;
        case u'\uA78B': {
            str += u'\uA78C';
        } break;
        case u'\uA78D': {
            str += u'\u0265';
        } break;
        case u'\uA790': {
            str += u'\uA791';
        } break;
        case u'\uA792': {
            str += u'\uA793';
        } break;
        case u'\uA796': {
            str += u'\uA797';
        } break;
        case u'\uA798': {
            str += u'\uA799';
        } break;
        case u'\uA79A': {
            str += u'\uA79B';
        } break;
        case u'\uA79C': {
            str += u'\uA79D';
        } break;
        case u'\uA79E': {
            str += u'\uA79F';
        } break;
        case u'\uA7A0': {
            str += u'\uA7A1';
        } break;
        case u'\uA7A2': {
            str += u'\uA7A3';
        } break;
        case u'\uA7A4': {
            str += u'\uA7A5';
        } break;
        case u'\uA7A6': {
            str += u'\uA7A7';
        } break;
        case u'\uA7A8': {
            str += u'\uA7A9';
        } break;
        case u'\uA7AA': {
            str += u'\u0266';
        } break;
        case u'\uA7AB': {
            str += u'\u025C';
        } break;
        case u'\uA7AC': {
            str += u'\u0261';
        } break;
        case u'\uA7AD': {
            str += u'\u026C';
        } break;
        case u'\uA7AE': {
            str += u'\u026A';
        } break;
        case u'\uA7B0': {
            str += u'\u029E';
        } break;
        case u'\uA7B1': {
            str += u'\u0287';
        } break;
        case u'\uA7B2': {
            str += u'\u029D';
        } break;
        case u'\uA7B3': {
            str += u'\uAB53';
        } break;
        case u'\uA7B4': {
            str += u'\uA7B5';
        } break;
        case u'\uA7B6': {
            str += u'\uA7B7';
        } break;
        case u'\uA7B8': {
            str += u'\uA7B9';
        } break;
        case u'\uA7BA': {
            str += u'\uA7BB';
        } break;
        case u'\uA7BC': {
            str += u'\uA7BD';
        } break;
        case u'\uA7BE': {
            str += u'\uA7BF';
        } break;
        case u'\uA7C0': {
            str += u'\uA7C1';
        } break;
        case u'\uA7C2': {
            str += u'\uA7C3';
        } break;
        case u'\uA7C4': {
            str += u'\uA794';
        } break;
        case u'\uA7C5': {
            str += u'\u0282';
        } break;
        case u'\uA7C6': {
            str += u'\u1D8E';
        } break;
        case u'\uA7C7': {
            str += u'\uA7C8';
        } break;
        case u'\uA7C9': {
            str += u'\uA7CA';
        } break;
        case u'\uA7D0': {
            str += u'\uA7D1';
        } break;
        case u'\uA7D6': {
            str += u'\uA7D7';
        } break;
        case u'\uA7D8': {
            str += u'\uA7D9';
        } break;
        case u'\uA7F5': {
            str += u'\uA7F6';
        } break;
        case u'\uFF21': {
            str += u'\uFF41';
        } break;
        case u'\uFF22': {
            str += u'\uFF42';
        } break;
        case u'\uFF23': {
            str += u'\uFF43';
        } break;
        case u'\uFF24': {
            str += u'\uFF44';
        } break;
        case u'\uFF25': {
            str += u'\uFF45';
        } break;
        case u'\uFF26': {
            str += u'\uFF46';
        } break;
        case u'\uFF27': {
            str += u'\uFF47';
        } break;
        case u'\uFF28': {
            str += u'\uFF48';
        } break;
        case u'\uFF29': {
            str += u'\uFF49';
        } break;
        case u'\uFF2A': {
            str += u'\uFF4A';
        } break;
        case u'\uFF2B': {
            str += u'\uFF4B';
        } break;
        case u'\uFF2C': {
            str += u'\uFF4C';
        } break;
        case u'\uFF2D': {
            str += u'\uFF4D';
        } break;
        case u'\uFF2E': {
            str += u'\uFF4E';
        } break;
        case u'\uFF2F': {
            str += u'\uFF4F';
        } break;
        case u'\uFF30': {
            str += u'\uFF50';
        } break;
        case u'\uFF31': {
            str += u'\uFF51';
        } break;
        case u'\uFF32': {
            str += u'\uFF52';
        } break;
        case u'\uFF33': {
            str += u'\uFF53';
        } break;
        case u'\uFF34': {
            str += u'\uFF54';
        } break;
        case u'\uFF35': {
            str += u'\uFF55';
        } break;
        case u'\uFF36': {
            str += u'\uFF56';
        } break;
        case u'\uFF37': {
            str += u'\uFF57';
        } break;
        case u'\uFF38': {
            str += u'\uFF58';
        } break;
        case u'\uFF39': {
            str += u'\uFF59';
        } break;
        case u'\uFF3A': {
            str += u'\uFF5A';
        } break;
        default: {
            str += ch;
        } break;
    }
}

bool Unicode::isSpace(char16 ch) {
    switch (ch) {
        case u'\u0000': [[fallthrough]];
        case u'\u0001': [[fallthrough]];
        case u'\u0002': [[fallthrough]];
        case u'\u0003': [[fallthrough]];
        case u'\u0004': [[fallthrough]];
        case u'\u0005': [[fallthrough]];
        case u'\u0006': [[fallthrough]];
        case u'\u0007': [[fallthrough]];
        case u'\u0008': [[fallthrough]];
        case u'\u0009': [[fallthrough]];
        case u'\u000A': [[fallthrough]];
        case u'\u000B': [[fallthrough]];
        case u'\u000C': [[fallthrough]];
        case u'\u000D': [[fallthrough]];
        case u'\u000E': [[fallthrough]];
        case u'\u000F': [[fallthrough]];
        case u'\u0010': [[fallthrough]];
        case u'\u0011': [[fallthrough]];
        case u'\u0012': [[fallthrough]];
        case u'\u0013': [[fallthrough]];
        case u'\u0014': [[fallthrough]];
        case u'\u0015': [[fallthrough]];
        case u'\u0016': [[fallthrough]];
        case u'\u0017': [[fallthrough]];
        case u'\u0018': [[fallthrough]];
        case u'\u0019': [[fallthrough]];
        case u'\u001A': [[fallthrough]];
        case u'\u001B': [[fallthrough]];
        case u'\u001C': [[fallthrough]];
        case u'\u001D': [[fallthrough]];
        case u'\u001E': [[fallthrough]];
        case u'\u001F': [[fallthrough]];
        case u'\u0020': [[fallthrough]];
        case u'\u007F': [[fallthrough]];
        case u'\u0080': [[fallthrough]];
        case u'\u0081': [[fallthrough]];
        case u'\u0082': [[fallthrough]];
        case u'\u0083': [[fallthrough]];
        case u'\u0084': [[fallthrough]];
        case u'\u0085': [[fallthrough]];
        case u'\u0086': [[fallthrough]];
        case u'\u0087': [[fallthrough]];
        case u'\u0088': [[fallthrough]];
        case u'\u0089': [[fallthrough]];
        case u'\u008A': [[fallthrough]];
        case u'\u008B': [[fallthrough]];
        case u'\u008C': [[fallthrough]];
        case u'\u008D': [[fallthrough]];
        case u'\u008E': [[fallthrough]];
        case u'\u008F': [[fallthrough]];
        case u'\u0090': [[fallthrough]];
        case u'\u0091': [[fallthrough]];
        case u'\u0092': [[fallthrough]];
        case u'\u0093': [[fallthrough]];
        case u'\u0094': [[fallthrough]];
        case u'\u0095': [[fallthrough]];
        case u'\u0096': [[fallthrough]];
        case u'\u0097': [[fallthrough]];
        case u'\u0098': [[fallthrough]];
        case u'\u0099': [[fallthrough]];
        case u'\u009A': [[fallthrough]];
        case u'\u009B': [[fallthrough]];
        case u'\u009C': [[fallthrough]];
        case u'\u009D': [[fallthrough]];
        case u'\u009E': [[fallthrough]];
        case u'\u009F': [[fallthrough]];
        case u'\u00A0': [[fallthrough]];
        case u'\u1680': [[fallthrough]];
        case u'\u2000': [[fallthrough]];
        case u'\u2001': [[fallthrough]];
        case u'\u2002': [[fallthrough]];
        case u'\u2003': [[fallthrough]];
        case u'\u2004': [[fallthrough]];
        case u'\u2005': [[fallthrough]];
        case u'\u2006': [[fallthrough]];
        case u'\u2007': [[fallthrough]];
        case u'\u2008': [[fallthrough]];
        case u'\u2009': [[fallthrough]];
        case u'\u200A': [[fallthrough]];
        case u'\u2028': [[fallthrough]];
        case u'\u2029': [[fallthrough]];
        case u'\u202F': [[fallthrough]];
        case u'\u205F': [[fallthrough]];
        case u'\u3000': {
            return true;
        } break;
        default: {
            return false;
        } break;
    }
}
