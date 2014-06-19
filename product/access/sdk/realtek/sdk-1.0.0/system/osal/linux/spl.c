/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : interrupt lock/unlock API
 *
 */

/*
 * Include Files
 */
#include <common/error.h>
#include <linux/spinlock.h>
#include <osal/spl.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
unsigned long flags;
/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_spl_spin_lock
 * Description:
 *      Interrupt lock function.
 * Input:
 *      pLock  - pointer buffer of interrupt lock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - interrupt lock success.
 *      RT_ERR_FAILED - fail to interrupt lock routine.
 * Note:
 *      None
 */
int32
osal_spl_spin_lock(osal_spinlock_t *pLock)
{
    //local_irq_disable();
    //(*pLock)++;
    spin_lock_irqsave(pLock, flags);

    return RT_ERR_OK;
} /* end of osal_spl_spin_lock */

/* Function Name:
 *      osal_spl_spin_unlock
 * Description:
 *      Interrupt unlock function.
 * Input:
 *      pLock  - pointer buffer of interrupt unlock
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK    - interrupt unlock success.
 * Note:
 *      None
 */
int32
osal_spl_spin_unlock(osal_spinlock_t *pLock)
{
    //(*pLock)--;
	//local_irq_enable();
    spin_unlock_irqrestore(pLock, flags);

    return RT_ERR_OK;
} /* end of osal_spl_spin_unlock */
