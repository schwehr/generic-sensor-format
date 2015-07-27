/********************************************************************
 *
 * Description : Internal only functions of GSF.
 *
 *   The functions in this header are only meant for internal use.
 *   for testing and iternal linking.  Do not include this header in
 *   your code.
 *
 * Restrictions/Limitations :
 *
 * Copyright 2015 Google Inc. All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *  This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * http://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html
 *
 ********************************************************************/

void LocalSubtractTimes(const struct timespec *base_time,
                        const struct timespec *subtrahend,
                        double *difference);
void LocalAddTimes(const struct timespec *base_time,
                   double delta_time,
                   struct timespec *sum_time);
