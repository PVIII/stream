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
    sender read() const { return sender{}; }
};

struct move_only_writer
{
    move_only_writer(move_only_writer&&)      = default;
    move_only_writer(const move_only_writer&) = delete;
    struct sender
    {
        void submit() {}
    };
    sender write([[maybe_unused]] int v) const { return sender{}; }
};

#endif // TESTS_HELPERS_CONSTRAINED_TYPES_HPP_
