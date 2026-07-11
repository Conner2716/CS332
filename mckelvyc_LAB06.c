#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINESIZE 1024
#define MAXLISTINGS 25000

struct listing {
	int id, host_id, minimum_nights, number_of_reviews, calculated_host_listings_count, availability_365;
	char *host_name, *neighbourhood_group, *neighbourhood, *room_type;
	float latitude, longitude, price;
};

struct listing getfields(char *line) {
	struct listing item;

	item.id = atoi(strtok(line, ","));
	item.host_id = atoi(strtok(NULL, ","));
	item.host_name = strdup(strtok(NULL, ","));
	item.neighbourhood_group = strdup(strtok(NULL, ","));
	item.neighbourhood = strdup(strtok(NULL, ","));
	item.latitude = atof(strtok(NULL, ","));
	item.longitude = atof(strtok(NULL, ","));
	item.room_type = strdup(strtok(NULL, ","));
	item.price = atof(strtok(NULL, ","));
	item.minimum_nights = atoi(strtok(NULL, ","));
	item.number_of_reviews = atoi(strtok(NULL, ","));
	item.calculated_host_listings_count = atoi(strtok(NULL, ","));
	item.availability_365 = atoi(strtok(NULL, ","));

	return item;
}

void writeStruct(FILE *fp, struct listing item) {
	fprintf(fp, "%d,%d,%s,%s,%s,%f,%f,%s,%.2f,%d,%d,%d,%d\n",
		item.id, item.host_id, item.host_name, item.neighbourhood_group,
		item.neighbourhood, item.latitude, item.longitude, item.room_type,
		item.price, item.minimum_nights, item.number_of_reviews,
		item.calculated_host_listings_count, item.availability_365);
}


int cmp_host_name(const void *a, const void *b) {
	struct listing *itemA = (struct listing *) a;
	struct listing *itemB = (struct listing *) b;
	return strcmp(itemA->host_name, itemB->host_name);
}


int cmp_price(const void *a, const void *b) {
	struct listing *itemA = (struct listing *) a;
	struct listing *itemB = (struct listing *) b;
	if (itemA->price < itemB->price) return -1;
	if (itemA->price > itemB->price) return 1;
	return 0;
}

int main(int argc, char *argv[]) {
	static struct listing list_items[MAXLISTINGS];
	static struct listing sorted_copy[MAXLISTINGS];
	char line[LINESIZE];
	int count;
	char *infile, *hostfile, *pricefile;

	infile    = (argc > 1) ? argv[1] : "listings.csv";
	hostfile  = (argc > 2) ? argv[2] : "sorted_by_host_name.csv";
	pricefile = (argc > 3) ? argv[3] : "sorted_by_price.csv";

	FILE *fptr = fopen(infile, "r");
	if (fptr == NULL) {
		fprintf(stderr, "Error reading input file %s\n", infile);
		exit(-1);
	}

	count = 0;
	while (fgets(line, LINESIZE, fptr) != NULL) {
		if (strncmp(line, "id,", 3) == 0)
			continue;
		if (count >= MAXLISTINGS) {
			fprintf(stderr, "Warning: too many listings, truncating at %d\n", MAXLISTINGS);
			break;
		}
		list_items[count++] = getfields(line);
	}
	fclose(fptr);

	FILE *fhost = fopen(hostfile, "w");
	if (fhost == NULL) {
		fprintf(stderr, "Error opening output file %s\n", hostfile);
		exit(-1);
	}
	FILE *fprice = fopen(pricefile, "w");
	if (fprice == NULL) {
		fprintf(stderr, "Error opening output file %s\n", pricefile);
		exit(-1);
	}

	memcpy(sorted_copy, list_items, count * sizeof(struct listing));
	qsort(sorted_copy, count, sizeof(struct listing), cmp_host_name);
	fprintf(fhost, "id,host_id,host_name,neighbourhood_group,neighbourhood,latitude,longitude,room_type,price,minimum_nights,number_of_reviews,calculated_host_listings_count,availability_365\n");
	for (int i = 0; i < count; i++)
		writeStruct(fhost, sorted_copy[i]);

	memcpy(sorted_copy, list_items, count * sizeof(struct listing));
	qsort(sorted_copy, count, sizeof(struct listing), cmp_price);
	fprintf(fprice, "id,host_id,host_name,neighbourhood_group,neighbourhood,latitude,longitude,room_type,price,minimum_nights,number_of_reviews,calculated_host_listings_count,availability_365\n");
	for (int i = 0; i < count; i++)
		writeStruct(fprice, sorted_copy[i]);

	fclose(fhost);
	fclose(fprice);

	printf("Read %d listings from %s\n", count, infile);
	printf("Wrote host_name-sorted output to %s\n", hostfile);
	printf("Wrote price-sorted output to %s\n", pricefile);

	return 0;
}