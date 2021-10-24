#include <Arduino.h>
#include <deneyap.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFiESP32.h>

char *ssid = "Wireless_N_8CA5";
char *pass = "meyankoku";

WiFiServer server(4444);

struct Node
{
  WiFiClient client;
  struct Node *next, *prev;
}  * head , * last; 

void add_client(WiFiClient new_client)
{
	struct Node * newNode = new Node;
	newNode->client = new_client;

  if(head == NULL && last == NULL)
	{
		head = last = newNode;
		last = newNode;
		head->prev = NULL;
		head->next = NULL;
	}
	else 
	{
		last->next = newNode;
		newNode->prev = last;
		last = newNode;
		last->next = NULL;
	}	

	Serial.print("Eklenen baglanti adresi : ");
	Serial.print(new_client.remoteIP());
	Serial.print("  port: ");
	Serial.println(new_client.remotePort());
}

void delete_node(struct Node * node)
{
	if(node == head)
	{
		if(node->next == NULL)
		{
			head = NULL;
			last = NULL;
		}
		else
		{
			head = node->next;
			head->prev = NULL;
		}
	}
	else if(node == last)
	{
		last = node->prev;
		last->next = NULL;
	}
	else
	{
		node->prev->next = node->next;
	}

	Serial.print("Silinen baglanti adresi : ");
	Serial.print(node->client.remoteIP());
	Serial.print("  port: ");
	Serial.println(node->client.remotePort());
	node->client.stop();
	free(node);
}

void update_clients()
{
	if(head == NULL && last == NULL) return;
	
	struct Node * node = head;
	
	while(node != NULL)
	{
		if(! node->client.connected())
		{
			delete_node(node);
		}

		while(node->client.available())
		{
			Serial.println(node->client.readStringUntil('\n'));
		}

		node = node->next;
	}

}

void setup()
{
	head = NULL;
	last = NULL;
	Serial.begin(115200);
	WiFi.begin(ssid,pass);
	
	unsigned long before_time,last_time;
	before_time = millis();
	while(WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(500);
	}

	last_time = millis();
	float  time = (float)(last_time - before_time) /(float) 1000;
	
	Serial.println();
	Serial.print("Baglanma suresi : "); 
	Serial.println(time);

	server.begin();
	
	Serial.println("Sunucu baslatildi.");
}

void loop()
{
	if(server.hasClient())
	{
		WiFiClient new_client = server.available();
		add_client(new_client);
	}

	update_clients();
}
