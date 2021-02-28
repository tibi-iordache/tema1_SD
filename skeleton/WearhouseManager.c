// IORDACHE TIBERIU-MIHAI 312CD
#include <stdio.h>
#include "WearhouseManager.h"


Package *create_package(long priority, const char* destination){
	Package* pachet = (Package*) malloc(sizeof(Package));
	// verificare alocare
	if (pachet == NULL) {
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	pachet->priority = priority;

	// duplichez destinatia daca e cazul
	if (destination != NULL) {
		pachet->destination = strndup(destination, strlen(destination) + 1);
		// verificare alocare
		if (pachet->destination == NULL) {
			free(pachet);
			printf("Eroare alocare memorie\n");
			exit(1);
		}
	}
		
	else 
		pachet->destination = NULL;

	return pachet;
}

void destroy_package(Package* package){
	// dealoc spatiul pt destinatie
	if (package->destination != NULL)
		free(package->destination);

	// dealoc pachetul
	free(package);
}

Manifest* create_manifest_node(void){
	Manifest* man = (Manifest*) malloc(sizeof(Manifest));
	// verificare alocare
	if (man == NULL) {
		printf("Eroare alocare memorie\n");
		return NULL;
	}

	// initializez campurile la NULL
	man->package = NULL;
	man->next = man->prev = NULL;

	return man;
}

void destroy_manifest_node(Manifest* manifest_node){
	// daca nodul contine un pachet, dealoc
	if (manifest_node->package != NULL) 
		destroy_package(manifest_node->package);

	// refac legaturile
	if (manifest_node->next != NULL) 
		manifest_node->next->prev = manifest_node->prev;
	if (manifest_node->prev != NULL) 
		manifest_node->prev->next = manifest_node->next;

	free(manifest_node);
}

Wearhouse* create_wearhouse(long capacity){
	// guard
	if(capacity == 0) 
		return NULL;
		
	Wearhouse* depozit = (Wearhouse*) malloc(sizeof(Wearhouse));
	// verific alocarea
	if (depozit == NULL) {
		printf("Eroare alocare memorie\n");
		return NULL;
	}

	depozit->size = 0;

	depozit->capacity = capacity;

	// aloc spatiu pt vectorul de structuri
	depozit->packages = (Package**) malloc(capacity * sizeof(Package));
	// verificare alocare 
	if (depozit->packages == NULL) {
		free(depozit);
		printf("Eroare alocare memorie\n");
		return NULL;
	}

	// initializez campurile la NULL
	long i;
	for(i = 0; i < depozit->capacity; i++)
		depozit->packages[i] = NULL;

	return depozit;
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}

	free(line);


	fclose(fp);
	return w;

	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){
	if (w->size == 0) 
		return 1;
	else
		return 0;
}

long wearhouse_is_full(Wearhouse *w){
	if (w->size == w->capacity) 
		return 1;
	else	
		return 0;
}

long wearhouse_max_package_priority(Wearhouse *w){
	// guard
	if (w->capacity == 0)
		return 0;

	//parcurg vectorul de pachete
	long i, max = 0;
	for (i = 0; i < w->size; i++)
		// daca gasesc un pachet cu prioritate mai mare, actualizez max
		if (w->packages[i]->priority > max) 
			max = w->packages[i]->priority;
	
	return max;
}

long wearhouse_min_package_priority(Wearhouse *w){
	// guard
	if (w->capacity == 0)
		return 0;

	// parcurg vectorul de pachete
	long i, min = wearhouse_max_package_priority(w);
	for (i = 0; i < w->size; i++)
		// daca gasesc un pachet cu prioritate mai mica, actualizez min
		if (w->packages[i]->priority < min) 
			min = w->packages[i]->priority;
	
	return min;
}


void wearhouse_print_packages_info(Wearhouse *w){
	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){
	// parcurg vectorul de pachete
	long i;
	for(i = 0; i < wearhouse->size; i++)
		// daca exista un pachet la pozitia i, il distrug
		if (wearhouse->packages[i] != NULL)
			destroy_package(wearhouse->packages[i]);
	
	// eliberez memoria pt vectorul de pachete
	free(wearhouse->packages);

	free(wearhouse);
}


Robot* create_robot(long capacity){
	Robot* robot = (Robot*) malloc(sizeof(Robot));
	// verificare alocare
	if (robot == NULL) {
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	// initializez campurile
	robot->manifest = NULL;

	robot->size = 0;

	robot->capacity = capacity;	

	robot->next = NULL;

	return robot;
}

int robot_is_full(Robot* robot){
	if (robot->size == robot->capacity)
		return 1;
	else
		return 0;
}

int robot_is_empty(Robot* robot){
	if (robot->size == 0) 
		return 1;
	else
		return 0;
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){
	// parcurg vectorul de pachete
	long i;
	for(i = 0; i < w->capacity; i++)
		if (w->packages[i] != NULL)
			// returnez primul pachet cu prioritatea ceruta
			if (w->packages[i]->priority == priority)
				return w->packages[i];

	// daca nu s-a gasit, returnez NULL
	return NULL;
}

void robot_remove_wearhouse_package(Wearhouse *w, Package* package){
	// parcurg vectorul de pachete
	long i;
	for(i = 0; i < w->size; i++)
		if (w->packages[i] == package) {	
			// am gasit pachetul pe care vreau sa il elimin
			
			// shiftez restul de elemente din depozit
			long j;
			for (j = i; j < w->size; j++) 
				w->packages[j] = w->packages[j + 1];

			if (w->size > 0)
				(w->size)--;
		}
}

void robot_load_one_package(Robot* robot, Package* package){
	// guard
	if (robot_is_full(robot))
		return;

	// creez nodul ce urmeaza sa fie introdus
	Manifest* node = create_manifest_node();
	node->package = package;

	// caz special: robotul este gol
	if (robot_is_empty(robot)) {
		robot->manifest = node;
		(robot->size)++;
		return;
	}

	// head reprezinta iteratorul pe care o sa il folosesc pt a ma plimba
	// prin lista manifest (nu este head-ul listei)
	Manifest* head = robot->manifest;

	// parcurg lista manifest
	while(1) {
		// ca sa ies din while(1)
		if (head == NULL) 
			return;

		// guard
		if (head->package == NULL) 
			return;
		
		// verific prima data prioritatea
		if (package->priority == head->package->priority) {
			// daca e egala, trec la verificarea destinatiei
			if (strcmp(package->destination, head->package->destination) < 0) {
				// trebuie adaug inaintea lui head

				// adaug la inceputul listei
				if (head->prev == NULL) {
					node->next = head;

					head->prev = node;
					
					// actualizez inceputul listei
					robot->manifest = node;
					
					(robot->size)++;
					return;
				}
				else {
					// adaug la mijloc sau final
					node->next = head;
					node->prev = head->prev;

					head->prev->next = node;
					head->prev = node;
					
					(robot->size)++;
					return;
				}
			}
			else {
				// trebuie sa adaug dupa head

				// caz special: pachetul trebuie pus dupa ultimul nod din lista
				if (head->next == NULL) {
					node->prev = head;

					head->next = node;
					
					(robot->size)++;
					return;
				}
				else {
					// ma deplasez la urmatorul element din manifest
					head = head->next;
				}
			}
		}
		else 
			// pachetul are prioritate diferita de head
			if (package->priority > head->package->priority) {
				// inserez pachetul inaintea lui head

				// adaug la inceputul listei
				if (head->prev == NULL) {
					node->next = head;

					head->prev = node;
					
					// actualizez inceputul listei
					robot->manifest = node;
					
					(robot->size)++;
					return;
				}
				else {
					// adaug la mijloc sau la final
					node->next = head;
					node->prev = head->prev;

					head->prev->next = node;
					head->prev = node;
					
					(robot->size)++;
					return;
				}
			}
			else {
				// caz special: pachetul trebuie pus dupa ultimul nod din lista
				if (head->next == NULL) {
					node->prev = head;

					head->next = node;
					
					(robot->size)++;
					return;
				}
				else {
					// ma deplasez la urmatorul element din manifest
					head = head->next;
				}
			}
	}
}

long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
	// guard
	if (robot != NULL && wearhouse != NULL) {
		long pachete_incarcate = 0;

		while(1) {
			// guard 1
			if (robot_is_full(robot))
				return pachete_incarcate;

			// guard 2
			if (wearhouse_is_empty(wearhouse))
				return pachete_incarcate;

			// caut cea mai mare prioritate din depozit
			long prioritate = wearhouse_max_package_priority(wearhouse);

			// caut pachetul cu cea mai mare prioritate
			Package* pachet =  robot_get_wearhouse_priority_package(wearhouse, prioritate);

			// guard
			if (pachet != NULL) {
				// incarc pachetul in robot
				robot_load_one_package(robot, pachet);

				// elimin pachetul din depozit
				robot_remove_wearhouse_package(wearhouse, pachet);
				
				pachete_incarcate++;
			}
		}

		return pachete_incarcate;
	}
	else 
		return 0;	
}	

Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	// guard
	if (robot == NULL)
		return NULL;
	
 	long maxim = 0;

	// iteratorul pt robot->manifest
	Manifest* head = robot->manifest;

	Package* rezultat;
	
	// parcug lista
	while(1) {
		// guard
		if (head == NULL)
			break;

		if (head->package != NULL) {
			if (strcmp(head->package->destination, destination) == 0) {
				// pachetul are aceeasi destinatie
				if (maxim < head->package->priority) {
					// devine maxim
					maxim = head->package->priority;

					rezultat = head->package;	
				}
			}
		}

		// iterez in continuare
		head = head->next;
	}
	return rezultat;
}

void destroy_robot(Robot* robot){
	while(1) {
		// guard
		if (robot->manifest == NULL)
			break;
		
		// copiez incepul listei
		Manifest* aux = robot->manifest;

		// ma deplasez
		robot->manifest = robot->manifest->next;

		// free la nodul precedent
		destroy_manifest_node(aux);
	}

	// robot->next = NULL;

	free(robot);
}

void robot_unload_packages(Truck* truck, Robot* robot){
	// guard
	if (truck == NULL || robot == NULL)
		return;
	
	if (robot_is_empty(robot)) 
		return;

	// iterator
	Manifest* head_robot = robot->manifest;
	
	while(1) {
		// guard sa ies din while
		if (head_robot == NULL) 
			return;

		if (truck_is_full(truck))
			return;
		
		if (strcmp(head_robot->package->destination, truck->destination) == 0) {
			//incarc pachetul in tir

			// copiez nodul
			Manifest* aux_mutare = head_robot;

			// il scot din lista robotului
			if (head_robot->next != NULL) 
				head_robot->next->prev = head_robot->prev;
			if (head_robot->prev != NULL) 
				head_robot->prev->next = head_robot->next;
			else 
				robot->manifest = head_robot->next;

			// scad size
			if (robot->size > 0) 
				(robot->size)--;
			
			// actualizez iteratorul
			head_robot = head_robot->next;

			// pun nodul extras in tir

			// caz special
			if (truck_is_empty(truck)) {
				// este singurul nod din tir
				truck->manifest = aux_mutare;

				// setez la null campurile pt a stii
				// care e inceputul si finalul listei
				truck->manifest->next = truck->manifest->prev = NULL;

				(truck->size)++;
			}
			else {
				// pun nodul in fata inceputului listei tirului
				truck->manifest->prev = aux_mutare;

				aux_mutare->next = truck->manifest;

				truck->manifest = aux_mutare;

				// setez la null campul de prev pt a stii care este inceputul
				truck->manifest->prev = NULL;

				(truck->size)++;
			}
		}
		else 
			// actualizez iteratorul
			head_robot = head_robot->next;
	}
}

// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}



Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){
	Truck* truck = (Truck*) malloc(sizeof(Truck));
	// verificare alocare
	if (truck == NULL) {
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	// setez campurile
	truck->manifest = NULL;
	
	truck->unloading_robots = NULL;

	// duplichez
	if (destination != NULL) {
		truck->destination = strndup(destination, strlen(destination) + 1);
		// verificare alocare
		if (truck->destination == NULL) {
			free(truck);
			printf("Eroare alocare memorie\n");
			exit(1);
		}
	}
	else	
		truck->destination = NULL;

	truck->size = 0;
	
	truck->capacity = capacity;
	
	truck->in_transit_time = 0;

	truck->transit_end_time = transit_time;

	truck->departure_time = departure_time;

	truck->next = NULL;

	return truck;
}

int truck_is_full(Truck *truck){
	if (truck->size == truck->capacity)
		return 1;
	else	
		return 0;
}

int truck_is_empty(Truck *truck){
	if (truck->size == 0)
		return 1;
	else
		return 0;
}

long truck_destination_robots_unloading_size(Truck* truck){
	long rezultat = 0;
	
	// iterator
	Robot* head = truck->unloading_robots;

	while(1) {
		// guard
		if (head == NULL)
			break;
		
		// daca robot nu contine nimic, ma deplasez la urmatorul
		if (robot_is_empty(head)) {
			head = head->next;
		}
		else {
			long numar_pachete = 0;

			// iterator pt manifestul unui robot
			Manifest* head_manifest = head->manifest;

			while(head_manifest != NULL) {
				if (strcmp(head_manifest->package->destination,truck->destination) == 0) {
					// am gasit pachet cu destinatia cautata
					numar_pachete++;

					// actualizez iteratorul
					head_manifest = head_manifest->next;
				}
				else	
					// ma deplasez
					head_manifest = head_manifest->next;
			}

			if (numar_pachete > 0) {
				// cresc unloading size
				rezultat = rezultat + (head->size);
			}
			// ma deplasez
			head = head->next;
		}
	}

	return rezultat;
}



void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}


void destroy_truck(Truck* truck){
	// distrug lista de manifest
	while(1) {
		// guard
		if (truck->manifest == NULL)
			break;
		
		// copiez nodul
		Manifest* aux_manifest = truck->manifest;

		// ma deplasez
		truck->manifest = truck->manifest->next;

		// distrug copia
		destroy_manifest_node(aux_manifest);
	}

	// distrug lista de roboti
	while(1) {
		// guard
		if (truck->unloading_robots == NULL)
			break;

		Robot* aux_robot = truck->unloading_robots;

		truck->unloading_robots = truck->unloading_robots->next;

		destroy_robot(aux_robot); 
	}

	// truck->next = NULL;

	// eliberez destinatia
	if (truck->destination != NULL)
		free(truck->destination);
	
	free(truck);
}


Parkinglot* create_parkinglot(void){
	Parkinglot* parcare = (Parkinglot*) malloc(sizeof(Parkinglot));
	// verific alocarea
	if (parcare == NULL) {
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	//initializez campurile
	parcare->arrived_trucks = create_truck(NULL, 0, 0, 0);

	if (parcare->arrived_trucks == NULL) {
		free(parcare);
		printf("Eroare alocare memorie\n");
		exit(1);
	}
	// setez santinela
	parcare->arrived_trucks->next = parcare->arrived_trucks;

	parcare->departed_trucks = create_truck(NULL, 0, 0, 0);

	if (parcare->departed_trucks == NULL) {
		destroy_truck(parcare->arrived_trucks);
		free(parcare);
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	parcare->departed_trucks->next = parcare->departed_trucks;

	parcare->pending_robots = create_robot(0);

	if (parcare->pending_robots == NULL) {
		destroy_truck(parcare->departed_trucks);
		destroy_truck(parcare->arrived_trucks);
		free(parcare);
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	parcare->pending_robots->next = parcare->pending_robots;

	parcare->standby_robots = create_robot(0);

	if (parcare->standby_robots == NULL) {
		destroy_robot(parcare->pending_robots);
		destroy_truck(parcare->departed_trucks);
		destroy_truck(parcare->arrived_trucks);
		free(parcare);
		printf("Eroare alocare memorie\n");
		exit(1);
	}

	parcare->standby_robots->next = parcare->standby_robots;

	return parcare;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
	// guard
	if (parkinglot == NULL || robot == NULL) 
		return;
	
	// verific daca robotul este gol
	if (robot_is_empty(robot)) {
		// adaug in standby

		// iterator
		Robot* head = parkinglot->standby_robots->next;

		// daca lista este goala
		if (parkinglot->standby_robots->next == parkinglot->standby_robots) {
			parkinglot->standby_robots->next = robot;
			robot->next = parkinglot->standby_robots;
			return;
		}

		while(1) {
			// guard
			if (head == parkinglot->standby_robots) 
				return;
			else {
				if (robot->capacity > head->capacity) {
					// adaug in capul listei
						robot->next = head;
						parkinglot->standby_robots->next = robot;
						return;
				}
				else {
					if (robot->capacity < head->capacity && head->next == parkinglot->standby_robots) {
						// adaug la final robotul
						robot->next = parkinglot->standby_robots;
						head->next = robot;
						return;
					}
					else {
						head = head->next;
					}	
				}
			}
		}
	}
	else {
		// adaug in pending

		// iterator
		Robot* head = parkinglot->pending_robots->next;

		// lista e goala
		if (parkinglot->pending_robots->next == parkinglot->pending_robots) {
			parkinglot->pending_robots->next = robot;
			robot->next = parkinglot->pending_robots;
			return;
		}

		while(1) {
			// guard
			if (head == parkinglot->pending_robots) 
				return;
			else {
				if (robot->size > head->size) {
					// adaug in capul listei
						robot->next = head;
						parkinglot->pending_robots->next = robot;
						return;
				}
				else {
					if (robot->size < head->size && head->next == parkinglot->pending_robots) {
						// adaug la final robotul
						head->next = robot;
						robot->next = parkinglot->pending_robots;
						return;
					}
					else {
						head = head->next;
					}	
				}
			}
		}
	}
}

void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){
	// guard
	if (parkinglot == NULL || robot == NULL)
		return;
	
	if (robot_is_empty(robot)) {
		// il caut in lista de standby

		// iteratori
		Robot* head = parkinglot->standby_robots->next;
		Robot* head_prev = parkinglot->standby_robots; // santinela

		while(1) {
			// guard
			if (head == parkinglot->standby_robots) 
				return;
			
			if (head == robot) {
				// elimin
				
				// daca e la inceput
				if (head_prev == parkinglot->standby_robots) {
					parkinglot->standby_robots->next = head->next;
					return;
				}
				else	
					// daca e la final
					if (head->next == parkinglot->standby_robots) {
						head = head_prev;
						head_prev->next = parkinglot->standby_robots;
						return;
					}
					else {
						// daca e la jumate
						head_prev->next = head->next;
						head = head->next;
						return;
					}
			}
			else {
				head_prev = head;
				head = head->next;
			}
		}
	}
	else {
		// il caut in lista de pending

		// iteratori
		Robot* head = parkinglot->pending_robots->next;
		Robot* head_prev = parkinglot->pending_robots; // santinela

		while(1) {
			// guard
			if (head == parkinglot->pending_robots) 
				return;
			
			if (head == robot) {
				// elimin

				// daca e la inceput
				if (head_prev == parkinglot->pending_robots) {
					parkinglot->pending_robots->next = head->next;
					destroy_robot(head);
					return;
				}
				else	
					// daca e la final
					if (head->next == parkinglot->pending_robots) {
						head = head_prev;
						head_prev->next = parkinglot->pending_robots;
						return;
					}
					else {
						// daca e la jumate
						head_prev->next = head->next;
						head = head->next;
						return;
					}
			}
			else {
				head_prev = head;
				head = head->next;
			}
		}
	}
}

int parckinglot_are_robots_peding(Parkinglot* parkinglot){
	if (parkinglot->pending_robots->next != parkinglot->pending_robots)
		return 1;
	else
		return 0;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	// guard
	if (parkinglot->arrived_trucks->next == parkinglot->arrived_trucks)
		return 1;

	// iterator
	Truck* head = parkinglot->arrived_trucks->next;

	int ok = 0;

	while(1) {
		// guard
		if (head == parkinglot->arrived_trucks)
			return ok;
		
		if (truck_is_empty(head)) {
			ok = 1;
			head = head->next;
		}
		else 
			return 0;
	}
}


int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){
	if (parkinglot->departed_trucks->next == parkinglot->departed_trucks)
		return 0;
	else 
		return 1;
}


void destroy_parkinglot(Parkinglot* parkinglot){
	while(1) {
		// guard
		if (parkinglot->arrived_trucks->next == parkinglot->arrived_trucks)
			break;

		// copie
		Truck* aux = parkinglot->arrived_trucks->next;

		// iterez
		parkinglot->arrived_trucks->next = parkinglot->arrived_trucks->next->next;

		// eliberez
		destroy_truck(aux);
	}
	// distrug santinela
	destroy_truck(parkinglot->arrived_trucks);
	
	while(1) {
		if (parkinglot->departed_trucks->next == parkinglot->departed_trucks)
			break;

		Truck* aux = parkinglot->departed_trucks->next;

		parkinglot->departed_trucks->next = parkinglot->departed_trucks->next->next;

		destroy_truck(aux);
	}
	destroy_truck(parkinglot->departed_trucks);

	while(1) {
		if (parkinglot->pending_robots->next == parkinglot->pending_robots)
			break;

		Robot* aux = parkinglot->pending_robots->next;

		parkinglot->pending_robots->next = parkinglot->pending_robots->next->next;

		destroy_robot(aux);
	}
	destroy_robot(parkinglot->pending_robots);
	
	
	while(1) {
		if (parkinglot->standby_robots->next == parkinglot->standby_robots)
			break;

		Robot* aux = parkinglot->standby_robots->next;

		parkinglot->standby_robots->next = parkinglot->standby_robots->next->next;

		destroy_robot(aux);
	}
	destroy_robot(parkinglot->standby_robots);
	
	free(parkinglot);
}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");

}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	// guard
	if (parkinglot == NULL || truck == NULL) 
		return;
	// Search through arrived list, if exists node is found remove it
	// Note: this must remove the node from the list, NOT deallocate it

	// iteratori
	Truck* head = parkinglot->arrived_trucks->next;
	Truck* head_prev = parkinglot->arrived_trucks;

	// guard
	if (parkinglot->arrived_trucks->next != parkinglot->arrived_trucks) {
		while(1) {
			// guard
			if (head == parkinglot->arrived_trucks) 
				break;
			
			if (head == truck) {
				// trebuie scos din lista

				// nodul e la inceput
				if (head_prev == parkinglot->arrived_trucks) {
					parkinglot->arrived_trucks->next = head->next;
					break;
				}
				else 
					// nodul e la final
					if (head->next == parkinglot->arrived_trucks) {
						head_prev->next = parkinglot->arrived_trucks;
						break;
					}
					else {
						// nodul e la mijloc
						head_prev->next = head->next;
						head = head->next;
						break;
					}
			}
			else {
				head_prev = head;
				head = head->next;
			}
		}
	}
	// l am eliminat din arrived

	// iteratori
	head = parkinglot->departed_trucks->next;
	head_prev = parkinglot->departed_trucks;

	// lista e goala
	if (head == parkinglot->departed_trucks) {
		parkinglot->departed_trucks->next = truck;
		truck->next = parkinglot->departed_trucks;
		return;
	}
	
	while(1) {
		if (head == parkinglot->departed_trucks) 
			return;
		else {
			if (truck->departure_time < head->departure_time) {
				// adaug truck in fata lui head
				
				// adaug la inceput
				if (head_prev == parkinglot->departed_trucks) {
					truck->next = head;
					parkinglot->departed_trucks->next = truck;
					return;
				}
				else {
					// adaug dupa primul sau inaintea ultimului
					truck->next = head;
					head_prev->next = truck;
					return;
				}
			}
			else {
				// adaug truck la finalul listei
				if (truck->departure_time > head->departure_time && head->next == parkinglot->departed_trucks) {
					head->next = truck;
					truck->next = parkinglot->departed_trucks;
					return;
				}
				else {
					head_prev = head;
					head = head->next;
				}
			}
		}
	}
}


void truck_arrived(Parkinglot *parkinglot, Truck* truck){
	// guard
	if(parkinglot == NULL || truck == NULL) 
		return;

	// Search through departed list, if exists node is found remove it
	// Note: this must remove the node not deallocate it
	
	// iteratori
	Truck* head = parkinglot->departed_trucks->next;
	Truck* head_prev = parkinglot->departed_trucks;

	// guard
	if (head != parkinglot->departed_trucks) {
		while(1) {
			// guard
			if (head == parkinglot->departed_trucks) 
				break;

			if (head == truck) {
				// trebuie sa il scot din lista

				// nodul e la inceput
				if (head_prev == parkinglot->departed_trucks) {
					parkinglot->departed_trucks->next = head->next;
					break;
				}
				else 
					// nodul e la final
					if (head->next == parkinglot->departed_trucks) {
						head = head_prev;
						head_prev->next = parkinglot->departed_trucks;
						break;
					}
					else {
						// nodul e la mijloc
						head_prev->next = head->next;
						head = head->next;
						break;
					}
			}
			else {
				head_prev = head;
				head = head->next;
			}
		}
	}

	// tirul a ajuns, ii golesc manifest
	while(1) {
		// guard
		if (truck->manifest == NULL)
			break;
		else {
			Manifest* aux = truck->manifest;

			truck->manifest = truck->manifest->next;

			destroy_manifest_node(aux);
		}
	}

	truck->size = 0;
	
	truck->in_transit_time = 0;

	truck->manifest = NULL;
	
	// iteratori
	head = parkinglot->arrived_trucks->next;
	head_prev = parkinglot->arrived_trucks;
	
	// lista e goala
	if (head == parkinglot->arrived_trucks) {
		parkinglot->arrived_trucks->next = truck;

		truck->next = parkinglot->arrived_trucks;

		return;
	}

	while(1) {
		// guard
		if (head == parkinglot->arrived_trucks)
			return;
		else {
			if (strcmp(truck->destination, head->destination) == 0) {
				// compar acum departure
				if (truck->departure_time < head->departure_time) {
					// adaug in fata lui head
						truck->next = head;
						parkinglot->arrived_trucks->next = truck;
						return;
				}
				else {
					// inserez la final
					if (truck->departure_time > head->departure_time && head->next == parkinglot->arrived_trucks) {
						head->next = truck;
						truck->next = parkinglot->arrived_trucks;
						return;
					}
					else {
						// ma deplasez
						head_prev = head;
						head = head->next;
					}
				}
			}
			else 
				if (strcmp(truck->destination, head->destination) < 0) {
					// are prioritate truck
					
					// la inceput
					if (head_prev == parkinglot->arrived_trucks) {
						truck->next = head;
						parkinglot->arrived_trucks->next = truck;
						return;
					}
					else {
						// in rest
						truck->next = head;
						head_prev->next = truck;
						return;
					}
				}
				else {
					// la final
					if (strcmp(truck->destination, head->destination) > 0 && head->next == parkinglot->arrived_trucks) {
						head->next = truck;
						truck->next = parkinglot->arrived_trucks;
						return;
					}
					else {
						// ma deplasez
						head_prev = head;
						head = head->next;
					}
				}
		}
	}
}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){
	while(1) {
		// guard
		if (truck->unloading_robots == NULL) 
			return;
		else {
			Robot* aux = truck->unloading_robots;

			truck->unloading_robots = truck->unloading_robots->next;
			
			parkinglot_add_robot(parkinglot, aux);
		}
	}
}


// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){
	// iterator
	Truck* head = parkinglot->arrived_trucks->next;

	while(1) {
		// guard
		if (head == parkinglot->arrived_trucks)
			return;
		else {
			if (head->departure_time == day_hour) {
				// daca nodul curent trebuie sa plece la ora precizata
				// il adaug in lista de departed

				Truck* aux = head;

				head = head->next;

				truck_transfer_unloading_robots(parkinglot, aux);

				truck_departed(parkinglot, aux);
			}
			else 
				head = head->next;
		}
	}
}

// Depends on parking_turck_arrived
void truck_update_transit_times(Parkinglot* parkinglot){
	// guard
	if (!parkinglot_are_trucks_in_transit(parkinglot))
		return;

	// iterator
	Truck* head = parkinglot->departed_trucks->next;

	while(1) {
		// guard
		if (head == parkinglot->departed_trucks) 
			return;
		else {
			Truck* aux = head;

			head = head->next;

			// actualizez timpul
			(aux->in_transit_time)++;

			// daca s a ajuns la transit_end_time, trec tirul la arrived
			if (aux->in_transit_time == aux->transit_end_time) 
				truck_arrived(parkinglot, aux);
		}
	}
}

void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}