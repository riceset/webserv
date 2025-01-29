/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmatsuba <rmatsuba@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 22:44:59 by rmatsuba          #+#    #+#             */
/*   Updated: 2024/12/13 10:40:40 by rmatsuba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP

#include "ASocket.hpp"

class Listener : public ASocket {
    private:
        Listener();
    public:
        Listener(int port);
        ~Listener();
        int getFd() const;
};

#endif
