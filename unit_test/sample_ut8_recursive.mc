def uplus(a, b) {
  local c;

  b = b + 1;

  if (a + b > 3)
    c = a + b;
  else
    c = uplus(a + 1, b);

  return c;
}

result = uplus(0, 0);

print result;