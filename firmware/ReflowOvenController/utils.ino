
// increment the value of a variable using minimun and maximun limits (byte version)
void circularList_incrementBy(byte *value, byte minimun, byte maximun, int incrementBy) {

  if (incrementBy>0) {

    if (*value+incrementBy > maximun) *value = minimun;
    else *value = *value+incrementBy;

  }
  else {

    if ( (*value < minimun) || (*value < (incrementBy*(-1)) )) *value = maximun;
    else *value = *value+incrementBy;

  }

}
