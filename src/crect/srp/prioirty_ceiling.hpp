//          Copyright Emil Fresk 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

/****************************************************************************
 *
 * This file is intended to be included after the definition of the system's
 * job list.
 *
 ****************************************************************************/

#include "kvasir/mpl/mpl.hpp"

#include "crect/details/job_resource.hpp"
#include "crect/details/job_resource_details.hpp"
#include "crect/details/job_resource_methods.hpp"

namespace crect
{
namespace details
{

/**
 * @brief Alias for the entire system's resource tree.
 *
 * @tparam  JobList   The system's job list.
 */
template <typename JobList>
using resource_tree = typename make_resource_tree<JobList>::result;

/**
 * @brief Finds a non-unique resource in the resource tree (implementation).
 *
 * @tparam  JobList   The system's job list.
 * @tparam  Resource  Resource to find.
 */
template <typename JobList, typename Resource>
struct find_resource_impl
{
  /* Searches the resource tree for the same Resource ID. */
  using f = kvasir::mpl::eager::find_if<resource_tree<JobList>,
                                    _same_resource_id<Resource>::template f >;

  static_assert(!std::is_same< f, kvasir::mpl::list<> >::value,
                "The resource in not registered in crect");

  static_assert(kvasir::mpl::eager::pop_front<f>::front::is_unique::value == false,
                "Found unique resource when normal was requested");
};

/**
 * @brief Finds a unique resource in the resource tree (implementation).
 *
 * @tparam  JobList   The system's job list.
 * @tparam  Resource  Resource to find.
 */
template <typename JobList, typename Resource>
struct find_unique_resource_impl
{
  /* Searches the resource tree for the same Resource ID. */
  using f = kvasir::mpl::eager::find_if<resource_tree<JobList>,
                                    _same_resource_id<Resource>::template f >;

  static_assert(!std::is_same< f, kvasir::mpl::list<> >::value,
                "The resource in not registered in crect");

  static_assert(kvasir::mpl::eager::pop_front<f>::front::is_unique::value == true,
                "Found normal resource when unique was requested");
};

/**
 * @brief Finds a non-unique resource in the resource tree (alias).
 *
 * @tparam  JobList   The system's job list.
 * @tparam Resource   Resource to find.
 */
template <typename JobList, typename Resource>
using find_resource = typename kvasir::mpl::eager::pop_front<
                        typename find_resource_impl<JobList, Resource>::f
                      >::front;

/**
 * @brief Finds a unique resource in the resource tree (alias).
 *
 * @tparam  JobList   The system's job list.
 * @tparam Resource   Resource to find.
 */
template <typename JobList, typename Resource>
using find_unique_resource = typename kvasir::mpl::eager::pop_front<
                        typename find_unique_resource_impl<JobList, Resource>::f
                      >::front;

template <typename... Ts>
struct job_to_priority
{
  static_assert(kvasir::mpl::eager::always_false<Ts...>::value, "Should not come here");
};

/**
 * @brief Converts a job to an integral_constant containing the job's priority.
 *
 * @tparam PRIO Priority.
 * @tparam ISR  ISR definition.
 * @tparam Res  Parameter pack of resources.
 */
template <unsigned PRIO, typename ISR, typename... Res>
struct job_to_priority< job<PRIO, ISR, Res...> > :
    kvasir::mpl::integral_constant<unsigned, PRIO>
{
};

/**
 * @brief Extracts the priorities of the jobs connected to the resource.
 *
 * @tparam  JobList   The system's job list.
 * @tparam Resource   Resource to get priority list from.
 */
template <typename JobList, typename Resource>
using resource_to_priority_list =
                kvasir::mpl::eager::join< kvasir::mpl::eager::transform<
                  kvasir::mpl::eager::join< typename find_resource<JobList,
                                                            Resource>::jobs >,
                  job_to_priority
                > >;

} /* END namespace details */

/**
 * @brief Extracts the priority ceiling of the jobs connected to the resource.
 *
 * @tparam  JobList   The system's job list.
 * @tparam Resource   Resource to get priority ceiling for.
 */
template <typename JobList, typename Resource>
using get_priority_ceiling =
                kvasir::mpl::eager::fold_right<
                  details::resource_to_priority_list<JobList, Resource>,
                  kvasir::mpl::integral_constant<unsigned, 0>,
                  kvasir::mpl::eager::max
                >;

} /* END namespace crect */
