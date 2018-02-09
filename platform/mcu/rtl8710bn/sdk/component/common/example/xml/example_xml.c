#include "FreeRTOS.h"
#include "task.h"
#include <platform_stdlib.h>
#include "xml.h"

static void example_xml_thread(void *param)
{
	/* Create XML document
	 * <Home:Light xmlns:Home="http://www.home.com" xmlns="http://www.device.com" fw_ver="1.0.0">
	 *     <Power>on</Power>
	 *     <Color>
	 *         <Red>255</Red>
	 *         <Green>255</Green>
	 *         <Blud>255</Blue>
	 *     </Color>
	 * </Home:Light>
	 */
	struct xml_node *light_node, *power_node, *color_node, *red_node, *green_node, *blue_node;

	// Creates element with (prefix, tag name, namespace uri), add element and text node as child
	light_node = xml_new_element("Home", "Light", "http://www.home.com");
	power_node = xml_new_element(NULL, "Power", NULL);
	xml_add_child(power_node, xml_new_text("on"));
	color_node = xml_new_element(NULL, "Color", NULL);
	red_node = xml_new_element(NULL, "Red", NULL);
	xml_add_child(red_node, xml_new_text("255"));
	green_node = xml_new_element(NULL, "Green", NULL);
	xml_add_child(green_node, xml_new_text("255"));
	blue_node = xml_new_element(NULL, "Blue", NULL);
	xml_add_child(blue_node, xml_new_text("255"));
	xml_add_child(light_node, power_node);
	xml_add_child(light_node, color_node);
	xml_add_child(color_node, red_node);
	xml_add_child(color_node, green_node);
	xml_add_child(color_node, blue_node);

	// Add or modify attributes
	xml_set_attribute(light_node, "xmlns", "http://www.device.com");
	xml_set_attribute(light_node, "fw_ver", "1.0.0");

	// Dump XML document to memory buffer, equal to xml_dump_tree_ex(node, NULL, 0, 0);
	char *dump_buf = xml_dump_tree(light_node);
	printf("\n%s\n", dump_buf);
	// Free dump buffer
	xml_free(dump_buf);
	// Dump XML document to memory buffer with prolog, new line, aligment
	dump_buf = xml_dump_tree_ex(light_node, "<?xml version=\"1.0\"?>", 1, 4);
	printf("\n%s\n", dump_buf);
	xml_free(dump_buf);

	// Delete XML tree to free memory
	xml_delete_tree(light_node);

	/* Parse XML document */
	char *doc = "\
		<?xml version=\"1.0\"?>\
		<!--sensor example-->\
		<Home:Sensor>\
			<Thermostat>\
				<Mode>auto</Mode>\
				<Temperature unit=\"celsius\">25.5</Temperature>\
			</Thermostat>\
		</Home:Sensor>";

	// Parse document buffer to XML tree. Prolog will be dropped
	struct xml_node *root = xml_parse(doc, strlen(doc));

	if(root) {
		dump_buf = xml_dump_tree_ex(root, NULL, 1, 4);
		printf("\n%s\n", dump_buf);
		xml_free(dump_buf);

		// Search by XPath, prefix and name in path should be matched
		struct xml_node_set *set = xml_find_path(root, "/Home:Sensor/Thermostat/Temperature");

		if(set->count) {
			printf("\nFind %d element by %s\n", set->count, "/Home:Sensor/Thermostat/Temperature");

			// Get XML tree search result 0
			struct xml_node *temperature_node = set->node[0];

			if(xml_is_text(temperature_node->child)) {
				// Get text
				printf("Temperature[0] is %s\n", temperature_node->child->text);
			}

			// Get attribute
			char *unit = xml_get_attribute(temperature_node, "unit");
			printf("Unit is \"%s\"\n", unit);
			// Free attribute search result
			xml_free(unit);
		}

		// Delete XML tree search result to free memory
		xml_delete_set(set);

		xml_delete_tree(root);
	}
	else {
		printf("Xml parse failed\n");
	}

	vTaskDelete(NULL);
}

void example_xml(void)
{
	if(xTaskCreate(example_xml_thread, ((const char*)"example_xml_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

