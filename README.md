## Poly1305
The Poly1305 algorithm is a MAC (message authentication code) that acts as a
digital signature between two parties with a shared secret. It is used by
TLS 1.3 to sign encrypted data packets to ensure that they have not been
modified by a malicious man-in-the-middle while in transit.

Given a 32-byte one-time key and an arbitrary length message, the Poly1305 algorithm
generates a 16-byte "tag" (signature) for the message. The algorithm itself is
actually pretty simple. Note that all operations should be performed using
a constant time arithmetic libary to avoid leaking information about the key.

The specification for this MAC can be found 
[here](https://datatracker.ietf.org/doc/html/rfc8439#section-2.5).
```
r = key[0:16] & 0x0ffffffc0ffffffc0ffffffc0fffffff
s = key[16:32]
p = 2^130 - 5
for 16-byte block b in message
    a = (a + b) * r % p
tag = a + s
```
