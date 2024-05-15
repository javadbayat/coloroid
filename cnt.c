#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ultostr(unsigned long value, char *ptr, int base) {
	unsigned long t = 0, res = 0;
	unsigned long tmp = value;
	int count = 0;
	
	if (!ptr)
		return NULL;
	
	if (!tmp)
		count++;
	
	while (tmp > 0) {
		tmp = tmp / base;
		count++;
	}
	
	ptr += count;
	*ptr = 0;
	
	do {
		res = value - base * (t = value / base);
		if (res < 10)
			*(--ptr) = '0' + res;
		else if ((res >= 10) && (res < base))
			*(--ptr) = 'a' - 10 + res;
	} while ((value = t));
	
	return ptr;
}

int main(int argc, char **argv) {
	char *ipAddress, *buffer = NULL;
	
	if (argc < 2) {
		FILE *fdCommand = popen("ifconfig | grep inet | tail -1 | awk '{print $2}'", "r");
		if (!fdCommand) {
			printf("Failed to execute a command to retrieve private IP address\n");
			return 1;
		}
		
		buffer = (char *) malloc(128 * sizeof(char));
		if (!buffer) {
			printf("Out of memory\n");
			pclose(fdCommand);
			return 1;
		}
		
		ipAddress = fgets(buffer, 128 * sizeof(char), fdCommand);
		pclose(fdCommand);
		if (!ipAddress) {
			printf("Failed to retrieve private IP address\n");
			free(buffer);
			return 1;
		}
	}
	else
		ipAddress = argv[1];
	
	int nDots = 0, i;
	int ipAddressLen = strlen(ipAddress);
	char *parts[4] = { ipAddress, NULL, NULL, NULL };
	for (i = 0; i < ipAddressLen; i++) {
		if (ipAddress[i] == '.') {
			ipAddress[i] = 0;
			
			if (nDots++ == 3) {
				printf("Invalid IP address\n");
				
				if (buffer)
					free(buffer);
				
				return 1;
			}
			
			parts[nDots] = ipAddress + i + 1;
		}
	}
	
	if (nDots < 3) {
		printf("Invalid IP address\n");
		
		if (buffer)
			free(buffer);
		
		return 1;
	}
	
	int ipBytes[4];
	ipBytes[0] = atoi(parts[0]);
	ipBytes[1] = atoi(parts[1]);
	ipBytes[2] = atoi(parts[2]);
	ipBytes[3] = atoi(parts[3]);
	
	if (buffer)
		free(buffer);
	
	unsigned long rawIPAddress;
	rawIPAddress = ipBytes[0] << 24;
	rawIPAddress |= ipBytes[1] << 16;
	rawIPAddress |= ipBytes[2] << 8;
	rawIPAddress |= ipBytes[3];
	
	char deviceToken[16];
	ultostr(rawIPAddress, deviceToken, 36);
	
	printf("Your Device Token: %s\n", deviceToken);
	
	return 0;
}