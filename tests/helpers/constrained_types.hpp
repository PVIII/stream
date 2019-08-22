/*!
 * @author    Patrick Wang-Freninger <github@freninger.at>
 * @copyright MIT
 * @date      2019
 * @link      github.com/PVIII/stream
 */

#ifndef TESTS_HELPERS_CONSTRAINED_TYPES_HPP_
#define TESTS_HELPERS_CONSTRAINED_TYPES_HPP_

struct move_only
{
    move_only(move_only&&)      = default;
    move_only(const move_only&) = delete;
};

#endif // TESTS_HELPERS_CONSTRAINED_TYPES_HPP_
