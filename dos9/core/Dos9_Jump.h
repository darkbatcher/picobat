#ifndef DOS9_JUMP_H_INCLUDED
#define DOS9_JUMP_H_INCLUDED


/* automaticly jump to a label
    - lpLabelName : the label name include ':'
    - lpFileName : (Optional) the file to search into */
int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName);


#endif // DOS9_JUMP_H_INCLUDED
