#include <Controller.hpp>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
int main() {
	Controller ctrl;
//	HttpResponse paska;
//	paska.create_responseline(500);
	ctrl.controlLoop();
}
