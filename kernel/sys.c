#include <errno.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <string.h>

char encryptKey[513];
int keyLen;
int hiddenLocation;
int keySet;

int sys_ftime()
{
	return -ENOSYS;
}

int sys_mknod()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_mount()
{
	return -ENOSYS;
}

int sys_umount()
{
	return -ENOSYS;
}

int sys_ustat(int dev,struct ustat * ubuf)
{
	return -1;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setgid(int gid)
{
	if (current->euid && current->uid)
		if (current->gid==gid || current->sgid==gid)
			current->egid=gid;
		else
			return -EPERM;
	else
		current->gid=current->egid=gid;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

int sys_setuid(int uid)
{
	if (current->euid && current->uid)
		if (uid==current->uid || current->suid==current->uid)
			current->euid=uid;
		else
			return -EPERM;
	else
		current->euid=current->uid=uid;
	return 0;
}

int sys_stime(long * tptr)
{
	if (current->euid && current->uid)
		return -1;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (!tbuf)
		return jiffies;
	verify_area(tbuf,sizeof *tbuf);
	put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
	put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
	put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
	put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
	    end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->uid && current->euid)
		return -EPERM;
	if (current->leader)
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_oldolduname(void* v)
{
	printk("calling obsolete system call oldolduname\n");
	return -ENOSYS;
//	return (0);
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux 0.01-3.x","nodename","release ","3.x","i386"
	};
	int i;

	if (!name) return -1;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return (0);
}

int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

int sys_null(int nr)
{
	static int prev_nr=-2;
	if (nr==174 || nr==175) return -ENOSYS;

	if (prev_nr!=nr)
	{
		prev_nr=nr;
//		printk("system call num %d not available\n",nr);
	}
	return -ENOSYS;
}

int strCopy(char to[], char from[])
{

	int i = 0;

	while(get_fs_byte(from + i) != '\0') {
		to[i] = get_fs_byte(from + i);
		i++;
	}
	to[i] = '\0';
	return 1;

}


void __reverse(char *buf, int len)
{
    int i, j;
    char c;
    for(i = 0, j = len - 1; i < j; ++i, --j)
    {
        c = buf[i];
        buf[i] = buf[j];
        buf[j] = c;
    }
}



int itoa(int n, char *buf)
{
    int i = 0, sign;
    if((sign = n) < 0)
        n = -n;
    do
    {
        buf[i++] = n % 10 + '0';
    } while((n /= 10) > 0);

    if(sign < 0)
        buf[i++] = '-';
    buf[i] = '\0';
    __reverse(buf, i);
    return i;
}

unsigned int hash(unsigned char *str) {
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*
char encryptKey[513]; //ovo je key
int keyLen;
 * */
int sys_keyset(char passphrase[])
{
	int i = 0;

	while(1) {
		if(get_fs_byte(passphrase + i) == '\0') break;
		i++;
		//ovo mi je strlen
	}


	switch(i) {
		case 0:
		    encryptKey[0] = 0;
		case 2: case 4: case 8: case 16: case 32: case 64: case 128: case 256: case 512:
		    strCopy(encryptKey, passphrase);
		    keyLen = i;
		    //printk("%s\n", encryptKey);
		    return 0;
		default:
		    return -EINVAL;
	}
}

int sys_keyclear()
{
	int i = sys_keyset("");
	return 0;
}
//znam da ce blok za sifrovanje da mi bude uvek 1024,
// verovatno ce poslednji da bude malo cudan zbog nula ali boze moj,
// nadam se da ce ostati zapisano u
// fajlu iako nece moci da se cita normalno enkriptovan fajl,
// u svakom slucaju bice enkriptovan...

/*
char encryptKey[513]; //ovo je key
int keyLen;
 * */



int sifrujBlok(char * sekvenca) {

    int i, j, k, sekvencaLen, key_len, rows, index;
    sekvencaLen = strlen(sekvenca); //duzina string-a
    key_len = keyLen; //duzina kljuca
    rows = sekvencaLen / key_len; //koliko ce redova da ima matrica
    if ((sekvencaLen % key_len) != 0) rows++;
    //ako poslednji red nije pun

    char matrix[rows][key_len], temp[key_len];
    strcpy(temp, encryptKey); //privremeni kljuc koji mi sluzi da bi mogao da ga unakazim da ne bi izgubio stari

    k = 0;

    for (i = 0; i < rows; i++) { //kroz sve redove
        for (j = 0; j < keyLen; j++) { //kroz sve kolone, dakle ide do key_len
            if(sekvenca[k] == NULL) {
                matrix[i][j] = 255; //ako je 0 onda je jebi ga (nista)
            } else {
                matrix[i][j] = sekvenca[k++];
            }
        }
    }
    //*Sad ovde imam ono string u matrici! :) *//*
    k = 0;

    for (i = 0; i < key_len; i++) {
        index = findMin(temp);
        for (j = 0; j < rows; j++) {
            //printk("%d = %d\n", sekvenca[k], matrix[j][index]);
            sekvenca[k++] = matrix[j][index];
        }
        sekvenca[k] = '\0';
    }
    return 0;
}

/*Autor algoritma je http://scanftree.com/programs/c/c-code-to-encrypt-decrypt-message-using-transposition-cipher/
 * algoritam je prilagodjen da radi u ovom slucaju kako treba ali logika je njegova*/

int findMin(char * temp) {

    int i,j,min,index;
    min=temp[0];
    index=0;

    for (j=0;j < keyLen;j++) { //prodjem kroz sve kljuceve u temp-u
        if(temp[j]<min && temp[j] > 0) {
            min=temp[j];
            index=j;
        }
    }

    temp[index]= 126;

    return (index);
}

int desifrujBlok(char *sekvenca){

    int i, j, k, l, sekvencaLen,  rows, index;
    sekvencaLen = strlen(sekvenca);
    rows = sekvencaLen / keyLen;

    char matrix[rows][keyLen];
    char temp[keyLen];

    strcpy(temp, encryptKey);

    j = 0;


    char a[sekvencaLen];

    k = 1;
    for (i = 0; sekvenca[i] != NULL; i++) {

        a[j] = sekvenca[i];
        j++;
        if(k % rows == 0) {
            a[j] = '\0';
            index = findMin(temp);
            for (l = 0; l < rows; l++) {
                matrix[l][index] = a[l];
            }
            j = 0;
        }
        k++;
    }

    k = 0;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < keyLen; j++) {
            if(matrix[i][j] == 255) sekvenca[k++] = 0;
            else sekvenca[k++] = matrix[i][j];
        }
    }
    sekvenca[k] = '\0';

    return 0;
}

/*Da ne bi pravio dva skoro identicna koda a razlikuje se samo algoritam
 * imam mode, koji moze biti 1 ili 2 (ne unosi korisnik)
 * na osnovu kog odredjujem da li se fajl enkriptuje ili dekriptuje!*/



int compare(char * str1, char * str2) {

    /*printk("\n\nCompare str1 -%s\n\n", str1);
    printk("\n\nCompare str2 -%s\n\n", str2);*/
    int i;

    //proveravam prve brojeve drugog stringa (stringa koji trazim jer mi je u prvom upisana is sifra)
    for(i = 0; i < strlen(str2); i++) {
        if(str1[i] != str2[i]) return 0;
    }

    return 1;

}

int checkFajlTxt(int i_num) {

    struct m_inode * hidden;
    struct buffer_head * bh;
    hidden = iget(0x301, 141); //getovao sam .fajl.txt sa i_num 141

    int diskIndex = bmap(hidden, hidden->i_size / 1024);
    bh = bread(hidden->i_dev, diskIndex);

    char num[10];
    itoa(i_num, num);

    int result = 0;
    int counter = 0;
    int p = 0;

    char readNum[523];
    while(bh->b_data[counter] != '\0') { //ova petlja ide char po char
        if(bh->b_data[counter] == '\n') { //kad naidje na \n
            //printk("strlen[readNum] = %d", strlen[readNum]);
            if(compare(readNum, num)) {
                result = 1;
                hiddenLocation = counter - strlen(readNum);
                break;
            } else {
                int reset;
                for(reset = 0; reset < 523; reset++)
                    readNum[reset] = '\0';
                p = 0;
            }
        }else {
            readNum[p++] = bh->b_data[counter];
        }
        counter++;
    }

    brelse(bh);
    iput(hidden);

    return result;

}

int ispisiHidden() {
    struct m_inode * hidden;
    struct buffer_head * bh;
    hidden = iget(0x301, 141); //getovao sam .fajl.txt sa i_num 141

    //pozicija na disku
    int diskIndex = bmap(hidden, hidden->i_size / 1024);
    bh = bread(hidden->i_dev, diskIndex);

   printk("\n\nEnkriptovani Fajlovi\n\n%s\n\n",  bh->b_data);

    return 0;
}

int upisiUHidden(int i_num) {

    struct m_inode * hidden;
    struct buffer_head * bh;
    hidden = iget(0x301, 141); //getovao sam .fajl.txt sa i_num 141

    //pozicija na disku
    int diskIndex = bmap(hidden, hidden->i_size / 1024);
    bh = bread(hidden->i_dev, diskIndex);

    //num zelim ovde da upisem na kraj fajla!
    char num[10];
    itoa(i_num, num);

    int hidLen = strlen(bh->b_data);

    int i;

    //upisao sam u poslednju poziciju na nizu broj
    for(i = 0; i < 10; i++) {
        if(i < strlen(num))
            bh->b_data[hidLen+i] = num[i];
        else {
            bh->b_data[hidLen+i] = ' '; //razmake necu citati nego cu samo da popunim prostor do kljuca. Verovatno ne moram ali da bi mi radio strlen da ne bi imao karaktere posle \0
        }
    }

    //imam i_num enkriptovanog fajla sad zelim da od pozicije 10 do kraja upisem sifru!

    int j;
    char pass[10];
    itoa(hash(encryptKey), pass);

    //treba da upisem na poziciji 10


    for(i = 0, j = hidLen + 10; i < strlen(pass); i++, j++) {
        bh->b_data[j] = pass[i];

    }
    bh->b_data[strlen(bh->b_data)] = '\n';

    //ispisiHidden();

    brelse(bh);
    iput(hidden);

    return 0;
}

int izbrisiHidden(int i_num) {

    struct m_inode * hidden;
    struct buffer_head * bh;
    hidden = iget(0x301, 141); //getovao sam .fajl.txt sa i_num 141

    //pozicija na disku
    int diskIndex = bmap(hidden, hidden->i_size / 1024);
    bh = bread(hidden->i_dev, diskIndex);

    //num zelim ovde da upisem na kraj fajla!
    char num[10];
    itoa(i_num, num);

    int pointer = hiddenLocation;
    int shiftNum = 0;

    for(;bh->b_data[pointer] != '\n'; pointer++) {
        //printk("%c", bh->b_data[pointer]);
        shiftNum++;
    }


    int leftPointer = hiddenLocation;
    int rightPointer = hiddenLocation+shiftNum+1;
    int ito = strlen(bh->b_data) - leftPointer;

    //printk("HL: %d | shiftNum: %d", hiddenLocation, shiftNum);
    int i;
    for(i = 0;  i < ito; i++) {
        //printk("menjam: |%c = %c\n", bh->b_data[leftPointer] , bh->b_data[rightPointer]);
        if(rightPointer >= strlen(bh->b_data)) {
            bh->b_data[leftPointer] = '\0';
        }else {
            bh->b_data[leftPointer] = bh->b_data[rightPointer];
        }

        leftPointer++;
        rightPointer++;
    }
    //ispisiHidden();
    return 0;
}



int proveriSifru() {
    struct m_inode * hidden;
    struct buffer_head * bh;
    hidden = iget(0x301, 141); //getovao sam .fajl.txt sa i_num 141

    //pozicija na disku
    int diskIndex = bmap(hidden, hidden->i_size / 1024);
    bh = bread(hidden->i_dev, diskIndex);

    int pointer = hiddenLocation + 10;

    int i;

    int j;
    char pass[10];
    itoa(hash(encryptKey), pass);

    for(i = 0; bh->b_data[pointer] != '\n'; i++, pointer++) {
        //printk("%c = %c\n", bh->b_data[pointer], encryptKey[i]);
        if(bh->b_data[pointer] != pass[i]) {
            return 0;
        }
    }

    return 1;

}

int fileCount(int i_num, int amount) {

    struct m_inode * dir;
    struct buffer_head * bh;

    int entries;
    int i;
    struct dir_entry * de;

    dir = iget(0x301, i_num); //getovao sam .fajl.txt sa i_num 141
    int diskIndex = bmap(dir, dir->i_size / 1024);
    bh = bread(dir->i_dev, diskIndex);


    entries = dir->i_size / (sizeof (struct dir_entry));

    //printk("FC entries = %d\n", entries);

    /*prva dva ce biti . i .. to je za trenutni direktorijum inode i za prethodni direktorijum inode
     * ostali ce biti I_NUM svega u direktorijumu! Mogu da zovem S_ISDIR kad napravim inode i da provberim
     * ako je dir rekurzivno opet zovi ovo sranje i dodaj u listu za enkripciju!*/
    de = (struct dir_entry *) bh->b_data;
    i = 2;
    de+=2;


    //printk("\n\nKontent fajla\n\n");

    while (i < entries) {

        struct m_inode * inode = iget(0x301, de->inode);

        if(S_ISDIR(inode->i_mode)) {
            amount = fileCount(de->inode, amount);
            amount++;
        }else amount++;

        de++;
        i++;
    }

    return amount;

}

int getAllInodes(int i_num, int amount, int * inodes) {

    struct m_inode * dir;
    struct buffer_head * bh;

    int entries;
    int i;
    struct dir_entry * de;

    dir = iget(0x301, i_num); //getovao sam .fajl.txt sa i_num 141
    //pozicija na disku
    int diskIndex = bmap(dir, dir->i_size / 1024);
    //printk("\ndiskIndex = %d\n", diskIndex);

    bh = bread(dir->i_dev, diskIndex);

    entries = dir->i_size / (sizeof (struct dir_entry));

    /*prva dva ce biti . i .. to je za trenutni direktorijum inode i za prethodni direktorijum inode
     * ostali ce biti I_NUM svega u direktorijumu! Mogu da zovem S_ISDIR kad napravim inode i da provberim
     * ako je dir rekurzivno opet zovi ovu glupost i dodaj u listu za enkripciju!*/
    de = (struct dir_entry *) bh->b_data;

    i = 0;
    de+=2;

    //printk("\n\nKontent fajla\n\n");

    while (i < (entries-2)) {

        struct m_inode * inode = iget(0x301, de->inode);

        if(S_ISDIR(inode->i_mode)) {
            amount = getAllInodes(de->inode, amount, inodes);
            inodes[amount] = de->inode;
            amount++;
        }else {
            inodes[amount] = de->inode;
            amount++;
        }

        de++;
        i++;
    }

    return amount;

}

int sortInodes(int mode, int * files, int fileAmount) {

    int i, j;

    for (i = 0; i < fileAmount; i++) {
        struct m_inode *i1 = iget(0x301, files[i]);
        if ((mode == 2 && (!S_ISDIR(i1->i_mode))) || mode == 1 && (S_ISDIR(i1->i_mode))) {
            //nek proveri koji su sve vec na poziciji!
            continue;
        }


        for (j = i; j < fileAmount; j++) {
            struct m_inode *i2 = iget(0x301, files[j]);

            if ((mode == 2 && (!S_ISDIR(i2->i_mode))) || mode == 1 && (S_ISDIR(i2->i_mode))) {
                int tmp = files[i];
                files[i] = files[j];
                files[j] = tmp;
                break;
            }
        }
    }
}

int encrr(struct m_inode * inode, int prt) {
    int nr;
    //int diskIndex;

    struct buffer_head *bh;
    int size = inode->i_size;


    while (1) {
        if (nr = bmap(inode, size / 1024)) {
            if (!(bh = bread(inode->i_dev, nr))) {
                break;
            }
        } else {
            break;
        }

        if (bh) {
                //printk("\nsifrujem; %s\n", bh->b_data);
                if(!S_ISDIR(inode->i_mode))
                    sifrujBlok(bh->b_data); //ovo je metoda za enkripciju koja prima char *
                upisiUHidden(inode->i_num);
                if(prt)
                printk("Enkripcija fajla %d je zavrsena!\n",inode->i_num);

            bh->b_dirt = 1;
            brelse(bh);
        }
        size += 1024;
    }
    return 0;
}

int decrr(struct m_inode * inode, int prt) {
    int nr;
    //int diskIndex;

    struct buffer_head *bh;
    int size = inode->i_size;

    while (1) {
        if (nr = bmap(inode, size / 1024)) {
            if (!(bh = bread(inode->i_dev, nr))) {
                break;
            }
        } else {
            break;
        }

        if (bh) {
                if (proveriSifru()) {
                    //printk("\ndesifrujem; %s\n", bh->b_data);
                    if(!S_ISDIR(inode->i_mode))
                        desifrujBlok(bh->b_data);
                    izbrisiHidden(inode->i_num);
                } else {
                    printk("Pogresan kljuc! Unesite kljuc koji je koriscen za enkriptovanje!\n");
                    break;
                }

                if(prt) {
                    printk("Dekripcija faja %d zavrsena!\n", inode->i_num);
                }

            bh->b_dirt = 1;
            brelse(bh);
        }
        size += 1024;
    }
    return 0;

}

int sys_encr(int fd, int mode) {

    if (encryptKey[0] == '\0'){

        printk("Ne postoji kljuc za enkripciju! \nIskoristite komandu keyset za setovanje kljuca za enkriptovanje. \n");
        return 0;
    }

    struct file * file;
    struct m_inode * inode;

    if (fd >= NR_OPEN || !(file = current->filp[fd])) {
        printk("Nije moguce otvoriti fajl! \n");
        return -EINVAL;
    }

    inode = file->f_inode;

    int fileAmount = fileCount(inode->i_num, 0);
    //if(fileAmount == 0) fileAmount++;
    fileAmount++;
    int files[fileAmount];


    //ispisiHidden();

    int s = getAllInodes(inode->i_num, 0, files);
    if(s == 0)
        files[0] = inode->i_num;
    else{
        files[fileAmount-1] = inode->i_num;
        sortInodes(mode, files, fileAmount);
    }

    int i;

    /*for (i = 0; i < fileAmount; i++) {
        printk("%d\n", files[i]);
    }*/
    //sad imam sve fajlove iz foldera ili ako nije folder imam jedan fajl!
    for(i = 0; i < fileAmount; i++) {

        inode = iget(0x301, files[i]);


        //hidden file i_num = 141
        int flagEncr = checkFajlTxt(inode->i_num);

        if ((mode == 1 && flagEncr == 0) || (mode == 2 && flagEncr == 1)) {
            if(mode == 1)
                encrr(inode, 1);
            else
                decrr(inode, 1);

        } else if (mode == 1 && flagEncr == 1) {
            printk("Fajl %d je vec enkriptovan\n",inode->i_num);
        } else {
            printk("Fajl %d nije enkriptovan!\n",inode->i_num);
        }

    }

    return 0;
}