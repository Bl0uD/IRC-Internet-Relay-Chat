# Checklist de validation IRC

Ce fichier regroupe des tests manuels a executer avec cases a cocher.

## 1) Lancement

- [ ] Build OK

```bash
make re
```

- [ ] Serveur demarre

```bash
./ircserv 12345 pp
```

Attendu: message de type "Server CONNECTED" puis attente de connexion.

## 2) Setup clients (3 terminaux clients)

Dans chaque terminal client:

```bash
nc localhost 12345
```

### Client A (operateur du channel)

- [ ] USER

```text
USER alice
```

- [ ] PASS

```text
PASS aa
```

- [ ] NICK

```text
NICK alice
```

Attendu: utilisateur enregistre.

### Client B

```text
USER bob
PASS bb
NICK bob
```

### Client C

```text
USER carol
PASS cc
NICK carol
```

## 3) Tests commandes de base

### HELP

- [ ]

```text
HELP
```

Attendu: liste des commandes.

### CHANNEL_ON (liste des channels)

- [ ] Sans channel cree

```text
CHANNEL_ON
```

Attendu: message indiquant qu aucun channel n existe.

## 4) JOIN / MODE / INVITE (cas principal)

### Creation channel par A

- [ ]

```text
JOIN test
```

Attendu: channel cree, A devient membre + operateur.

### Password mode par A

- [ ]

```text
MODE test k pass
```

Attendu: mot de passe du channel defini.

### B tente sans password (doit echouer)

- [ ]

```text
JOIN test
```

Attendu: refuse avec message demandant password ou invite.

### B tente avec mauvais password (doit echouer)

- [ ]

```text
JOIN test wrong
```

Attendu: refuse.

### B tente avec bon password (doit reussir)

- [ ]

```text
JOIN test pass
```

Attendu: B rejoint le channel.

### C est invite puis rejoint sans password (doit reussir)

Depuis A:

- [ ]

```text
INVITE test carol
```

Depuis C:

- [ ]

```text
JOIN test
```

Attendu: C rejoint le channel grace a l invite.

## 5) CHANNEL message

### Message channel par B

Depuis B:

- [ ]

```text
CHANNEL test salut a tous
```

Attendu: A et C recoivent le message.

### Client hors channel (D) ne peut pas parler dans test

Ouvrir Client D et enregistrer (USER/PASS/NICK), puis:

- [ ]

```text
CHANNEL test hello
```

Attendu: refuse car non membre du channel.

## 6) PRIVMSG

### Message direct B -> A

Depuis B:

- [ ]

```text
PRIVMSG alice coucou
```

Attendu: A recoit le message prive.

### Nick inexistant

Depuis B:

- [ ]

```text
PRIVMSG inconnu test
```

Attendu: erreur utilisateur introuvable.

## 7) TOPIC / MODE t / MODE i / MODE l

### TOPIC simple

Depuis A:

- [ ]

```text
TOPIC test test2
```

Attendu: topic renomme en test2.

### Restriction topic (mode t)

Depuis A:

- [ ]

```text
MODE test2 t
```

Puis depuis B (non operateur):

- [ ]

```text
TOPIC test2 test3
```

Attendu: refuse (non operateur).

### Invite-only (mode i)

Depuis A:

- [ ]

```text
MODE test2 i
```

Depuis D (non invite):

- [ ]

```text
JOIN test2
```

Attendu: refuse (invite-only).

### Limitation utilisateurs (mode l)

Depuis A:

- [ ]

```text
MODE test2 l 3
```

Attendu: limite fixee a 3.

## 8) KICK

### A kick B

Depuis A:

- [ ]

```text
KICK test2 bob
```

Attendu: B est retire du channel et recoit la notif.

### B essaie de kick (doit echouer)

Depuis B:

- [ ]

```text
KICK test2 carol
```

Attendu: refuse (non operateur).

## 9) Robustesse erreurs d arguments

- [ ]

```text
USER
PASS
NICK
JOIN
MODE
INVITE
KICK
TOPIC
CHANNEL
PRIVMSG
```

Attendu: chaque commande invalide renvoie un message d usage.

## 10) Nettoyage

- [ ] Fermer les clients (Ctrl+C)
- [ ] Arreter le serveur (Ctrl+C)
- [ ] Redemarrer et verifier qu un nouveau lancement fonctionne

```bash
./ircserv 12345 pp
```
