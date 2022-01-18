import heapq


h = []
heapq.heappush(h, (10.1, 0))
heapq.heappush(h, (1.1, 1))
print(h)
assert h == [(1.1, 1), (10.1, 0)]
