//
// Created by jules on 26/12/2024.
//

#ifndef REQUESTBODY_HPP
# define REQUESTBODY_HPP

#include <vector>
#include <sys/types.h>

class RequestBody {
	private:
		std::vector<u_char>	rawBody;
	protected:
		size_t				parsedLength;

	public:
		RequestBody();
		RequestBody(const RequestBody&) = default;
		virtual ~RequestBody() = default;

		RequestBody &operator=(const RequestBody&) = default;
		bool		operator==(const RequestBody&) const = default;
		bool		operator!=(const RequestBody&) const = default;

		virtual bool		addData(const std::vector<u_char>& data);

		[[nodiscard]] size_t								getCumulatedSize() const;
		[[nodiscard]] virtual const std::vector<u_char>&	getContent() const;

		virtual void	clearContent();
};

#endif
