int fast_sqrt(int n) {
    float fres; 
    // get starting point
    if (n < 0b1 << 16) {
        if (n < 0b1 << 8) {
            if (n < 0b1 << 4) {
                if (n < 0b1 << 2) {
                    fres  = 0b1 << 0;
                }
                else fres  = 0b1 << 1;
            }
            else {
                if (n < 0b1 << 6) {
                    fres  = 0b1 << 2;
                }
                else fres  = 0b1 << 3;
            }
        }
        else {
            if (n < 0b1 << 12) {
                if (n < 0b1 << 10) {
                    fres  = 0b1 << 4;
                }
                else fres  = 0b1 << 5;
            }
            else {
                if (n < 0b1 << 14) {
                    fres  = 0b1 << 6;
                }
                else fres  = 0b1 << 7;
            }
        }
    }
    else {
        if (n < 0b1 << 24) {
            if (n < 0b1 << 20) {
                if (n < 0b1 << 18) {
                    fres  = 0b1 << 8;
                }
                else fres  = 0b1 << 9;
            }
            else {
                if (n < 0b1 << 22) {
                    fres  = 0b1 << 10;
                }
                else fres  = 0b1 << 11;
            }
        }
        else {
            if (n < 0b1 << 28) {
                if (n < 0b1 << 26) {
                    fres  = 0b1 << 12;
                }
                else fres  = 0b1 << 13;
            }
            else {
                if (n < 0b1 << 30) {
                    fres  = 0b1 << 14;
                }
                else fres  = 0b1 << 15;
            }
        }
    }
    // bruteforce 4 iterations of newton raphson
    fres = (fres*fres + n)/(2*fres);
    // fres = (fres*fres + n)/(2*fres);
    // fres = (fres*fres + n)/(2*fres);
    fres = (fres*fres + n)/(2*fres); // with this iteration was 10x slower and max error was 1

    return (int)fres;
}



