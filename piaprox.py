# sine approximation test
import math

# Calculate sin(2pi)

x = 2*math.pi
s = 0

for i in range(500):
    factor = 1 if i%2 == 0 else -1
    r = (2 * i) + 1

    s += factor * (x**r) / math.factorial(r)

    print('Error: {}'.format(s))
    if abs(s) < 0.0001:
        break