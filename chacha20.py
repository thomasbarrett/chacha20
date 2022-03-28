import json
from base64 import b64encode, b64decode
from Crypto.Cipher import ChaCha20
from Crypto.Random import get_random_bytes

plaintext = b'Attack at dawn'
key = get_random_bytes(32)
nonce_rfc7539 = get_random_bytes(12)
cipher = ChaCha20.new(key=key, nonce=nonce_rfc7539)
ciphertext = cipher.encrypt(plaintext)

try:
    cipher = ChaCha20.new(key=key, nonce=nonce_rfc7539)
    plaintext = cipher.decrypt(ciphertext)
    print("The message was " + str(plaintext))
except (ValueError, KeyError):
    print("Incorrect decryption")