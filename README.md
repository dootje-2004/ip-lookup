# IP address lookup

We are given an HTTP request's IP address and need to compare it to a list
of IP addresses, both IPv4 and IPv6.
A typical use case is the verification of blacklisted IP addresses.

The address list is a text file with one IP address or IP range per line.
Each entry can be a decimal IPv4 address or a (possibly compressed) hexadecimal IPv6 address.
