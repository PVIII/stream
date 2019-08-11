/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_MOCKS_CALLBACK_HPP_
#define TESTS_MOCKS_CALLBACK_HPP_

namespace stream
{

struct write_callback_interface
{
    virtual void operator()(stream::error_code) = 0;
};

struct read_callback_interface
{
    virtual void operator()(stream::error_code, char) = 0;
};

struct range_callback_interface
{
	virtual void operator()(stream::error_code, std::size_t) = 0;
};
	
}

#endif /* TESTS_MOCKS_CALLBACK_HPP_ */
