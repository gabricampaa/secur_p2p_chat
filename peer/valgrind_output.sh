ubuntu@ip-172-31-40-134:~/secur_p2p_chat/peer$ valgrind ./main_beta
==1847== Memcheck, a memory error detector
==1847== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==1847== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==1847== Command: ./main_beta
==1847==
Custom PRNG initialized
RSA key successfully generated
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqY8s58y03bsxLHULrCP1
Wl/Q122xvxfSjykACQ9v2VflLTq53iC7eTQm35tZcZcLji4ZsCSZJIqbZuSmwSlX
WqN8b6em4Bm4Ot6lQDOuT2GVF5tTmcZUly/IDO0JtdoA1+O8+V0aXU8vfAKapGqF
+v5xkG/xcej5ChsEzwz4+zbNTvYuhTruIEBy+gGFiT8yka2PlUrH9Uy6epKgLkGa
Qu9b5ZgwXk3q+jxUZGI/M9wLl2tfTgBKPw8+XXB8sjfynyX9J9X8h5xu9nFij2rb
08GroXjiW1r748ZOVYCbeLdamK/zCKM/XdD/oZEWF0Pwsfo9Jxe0WQi/ikUnHvUU
IQIDAQAB
-----END PUBLIC KEY-----
Connesso al server. Inizio invio del file...
File inviato con successo.
Connesso al server. Inizio invio del file...
File inviato con successo.

Now let's get the peer stuff! Insert the other dude's PUBLIC ipv4: 13.53.168.45
--2024-10-30 16:49:15--  http://13.53.40.109:8080/home/ubuntu/apple//13.53.168.45-files/HostConfForPeer
Connecting to 13.53.40.109:8080... connected.
HTTP request sent, awaiting response... 200 No headers, assuming HTTP/0.9
Length: unspecified
Saving to: ‘HostConfForPeer’

HostConfForPeer                            [ <=>                                                                      ]     146  --.-KB/s    in 0s

2024-10-30 16:49:15 (8.68 MB/s) - ‘HostConfForPeer’ saved [146]

--2024-10-30 16:49:15--  http://13.53.40.109:8080/home/ubuntu/apple//13.53.168.45-files/public_key.pem
Connecting to 13.53.40.109:8080... connected.
HTTP request sent, awaiting response... 200 No headers, assuming HTTP/0.9
Length: unspecified
Saving to: ‘public_key.pem’

public_key.pem                             [ <=>                                                                      ]     451  --.-KB/s    in 0s

2024-10-30 16:49:15 (68.0 MB/s) - ‘public_key.pem’ saved [451]

--2024-10-30 16:49:15--  http://13.53.40.109:8080/home/ubuntu/apple//13.53.168.45-files/private_address
Connecting to 13.53.40.109:8080... connected.
HTTP request sent, awaiting response... 200 No headers, assuming HTTP/0.9
Length: unspecified
Saving to: ‘private_address’

private_address                            [ <=>                                                                      ]       8  --.-KB/s    in 0s

2024-10-30 16:49:15 (1.09 MB/s) - ‘private_address’ saved [8]


Warning: `/home/ubuntu/secur_p2p_chat/peer/.vpn-secrets/wg0_vpn.conf' is world accessible
wg-quick: `wg0_vpn' already exists
ACTIVACTING VPN...Current working directory: /home/ubuntu/secur_p2p_chat/peer
Concatenated path: /home/ubuntu/secur_p2p_chat/peer
==1847== Invalid read of size 1
==1847==    at 0x484F226: strlen (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1847==    by 0x10D173: concatenateStrings (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10DE4A: secureP2Pchat_simone (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10B368: main (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==  Address 0x1ffefff160 is on thread 1's stack
==1847==  984 bytes below stack pointer
==1847==
==1847== Invalid read of size 1
==1847==    at 0x484F234: strlen (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1847==    by 0x10D173: concatenateStrings (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10DE4A: secureP2Pchat_simone (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10B368: main (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==  Address 0x1ffefff161 is on thread 1's stack
==1847==  983 bytes below stack pointer
==1847==
==1847== Invalid read of size 1
==1847==    at 0x484F374: strcpy (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1847==    by 0x10D1DC: concatenateStrings (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10DE4A: secureP2Pchat_simone (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10B368: main (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==  Address 0x1ffefff160 is on thread 1's stack
==1847==  984 bytes below stack pointer
==1847==
==1847== Invalid read of size 1
==1847==    at 0x484F392: strcpy (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==1847==    by 0x10D1DC: concatenateStrings (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10DE4A: secureP2Pchat_simone (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==    by 0x10B368: main (in /home/ubuntu/secur_p2p_chat/peer/main_beta)
==1847==  Address 0x1ffefff161 is on thread 1's stack
==1847==  983 bytes below stack pointer
==1847==
Public IPv4 address of this machine: 13.60.193.95

Enter your name (max 20 char):IP address is: 0.0.0.0
Port is: 51810

*****At any point in time press the following:*****
1. Send message
0. Quit

Enter choice:1

Connection Failed
^C
Signal 2 received.
Warning: `/home/ubuntu/secur_p2p_chat/peer/.vpn-secrets/wg0_vpn.conf' is world accessible
[#] ip link delete dev wg0_vpn

Message sent.
==1847==
==1847== HEAP SUMMARY:
==1847==     in use at exit: 1,383 bytes in 13 blocks
==1847==   total heap usage: 9,521 allocs, 9,508 frees, 1,099,600 bytes allocated
==1847==
==1847== LEAK SUMMARY:
==1847==    definitely lost: 249 bytes in 4 blocks
==1847==    indirectly lost: 0 bytes in 0 blocks
==1847==      possibly lost: 336 bytes in 1 blocks
==1847==    still reachable: 798 bytes in 8 blocks
==1847==         suppressed: 0 bytes in 0 blocks
==1847== Rerun with --leak-check=full to see details of leaked memory
==1847==
==1847== For lists of detected and suppressed errors, rerun with: -s
==1847== ERROR SUMMARY: 66 errors from 4 contexts (suppressed: 0 from 0)
