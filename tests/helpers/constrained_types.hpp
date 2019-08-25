/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_HELPERS_CONSTRAINED_TYPES_HPP_
#define TESTS_HELPERS_CONSTRAINED_TYPES_HPP_

struct move_only_reader
{
    move_only_reader(move_only_reader&&)      = default;
    move_only_reader(const move_only_reader&) = delete;
    struct sender
    {
        int submit() { return 0; }
    };
    sender read() { return sender{}; }
};

#endif // TESTS_HELPERS_CONSTRAINED_TYPES_HPP_
