//
// Created by jules on 30/03/2025.
//

#ifndef REQUESTSTATE_HPP
# define REQUESTSTATE_HPP

typedef enum e_request_state {
	REQUEST_PARSING,
	REQUEST_CHUNK_RECEIVING,
	REQUEST_OK,
	REQUEST_INVALID,
	REQUEST_BODY_TOO_LARGE,
	REQUEST_LEN_REQUIRED,
	I_AM_A_TEAPOT,
	REQUEST_UNIMPLEMENTED,
	REQUEST_TIMEOUT,
	HTTP_VERSION_NOT_SUPPORTED,
	SERVER_ERROR
}	t_request_state;

#endif //REQUESTSTATE_HPP
