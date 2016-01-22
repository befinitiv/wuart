#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>



const float ts = 55e-6;
const float srate = 2e6;
const float decim = 10;



void main(void) {
	const float rate = srate / decim;
	uint8_t buf[512];
	size_t n;
	
	int sympos = 0;
	uint8_t sym = 0;

	int state = 0;
	int delay = 0;

	for(;;) {
		int i;
		n = read(STDIN_FILENO, buf, sizeof(buf));


		for(i=0; i<n; ++i) {
			uint8_t c = buf[i];
			switch(state) {
				//wait for start bit
				case 0:
					if(c == 1)
						state = 1;
						sym = 0;
						sympos = 0;
						delay = ts * rate * 1.5; //wait until we are in the middle of the first symbol bit
					break;

				case 1:
					if(delay-- == 0) {
						sym |= c;
						if(sympos == 7) {
							//printf("sym: %x \"%c\"\n", sym, sym);
							write(STDOUT_FILENO, &sym, 1);
							delay = ts * rate;
							state = 2;
						}
						else
						{
							sym <<= 1;
							sympos++;
							delay = ts * rate;
						}
					}
					break;

				//wait for symbol to end
				case 2:
					if(delay-- == 0)
						state = 0;

					default:
						break;
				}
			}

		}

	}

