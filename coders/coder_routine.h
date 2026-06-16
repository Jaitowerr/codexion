/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aitorres <aitorres@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by aitorres          #+#    #+#             */
/*   Updated: 2026/06/10 16:27:06 by aitorres         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_ROUTINE_H
# define CODER_ROUTINE_H

struct t_coder;

void *coder_executed(void *arg);

bool check_burnout(t_coder *self);

bool	compile(t_coder *self);

bool	release_and_cooldown(t_coder *self);

bool	do_debug(t_coder *self);

bool	do_refactor(t_coder *self);

bool	take_dongles_fifo(t_coder *self);
void	cleanup_on_burnout_both_fifo(t_dongle *first, t_dongle *second);
void	init_request_fifo(t_request *req, t_coder *self);
void	insert_in_queue_fifo(t_dongle *dongle, t_request *req);
void	wait_for_grant_or_burnout_fifo(t_dongle *dongle, t_request *req, t_coder *self);
void	handle_burnout_fifo(t_dongle *dongle, t_request *req);
void	remove_from_queue_fifo(t_dongle *dongle, t_request *req);
void	cleanup_on_burnout_single_fifo(t_dongle *first);
void	get_dongle_order_fifo(t_coder *self, t_dongle **first, t_dongle **second);

bool	take_dongles_edf(t_coder *self);
void	handle_immediate_dongle_acquisition(t_dongle *dongle, t_request *req, t_coder *self);
void	cleanup_on_burnout_both_edf(t_dongle *first, t_dongle *second);
void	init_request_edf(t_request *req, t_coder *self);
void	insert_in_queue_edf(t_dongle *dongle, t_request *req);
void	wait_for_grant_or_burnout_edf(t_dongle *dongle, t_request *req, t_coder *self);
void	remove_from_queue_edf(t_dongle *dongle, t_request *req);
void	handle_burnout_edf(t_dongle *dongle, t_request *req);
void	cleanup_on_burnout_both_edf(t_dongle *first, t_dongle *second);
void	cleanup_on_burnout_single_edf(t_dongle *first);
void	get_dongle_order_edf(t_coder *self, t_dongle **first, t_dongle **second);

void	wait_cooldown_fifo_edf(t_dongle *dongle, t_request *req, t_coder *self);

bool	sleep_with_burnout_check(t_coder *self, long ms);

#endif