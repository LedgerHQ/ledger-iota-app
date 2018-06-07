# IOTA App for Ledger Nano S

[![Build Status](https://travis-ci.org/IOTA-Ledger/blue-app-iota.svg?branch=master)](https://travis-ci.org/IOTA-Ledger/blue-app-iota)

Here we try to use natively available crypto logic to create IOTA seeds and sign transactions on the fly.<br>

**WARNING: It's currently heavily in alpha so don't transfer all your funds to this yet!**

***It is strongly recommended to take a few minutes to read this document to make sure you fully understand how IOTA and the Ledger Nano S work, and how they interact together.***

## Table of contents

* [Introduction](#introduction)
  + [Terminology](#terminology)
  + [Address Reuse](#address-reuse)
  + [IOTA Bundle](#iota-bundle)
  + [Parts of an IOTA Transaction](#parts-of-an-iota-transaction)
* [How Ledger Nano S Works](#how-ledger-nano-s-works)
* [IOTA Specific Considerations on the Ledger Nano S](#iota-specific-considerations-on-the-ledger-nano-s)
  + [IOTA User-Facing App Functions](#iota-user-facing-app-functions)
    - [Functions](#functions)
    - [Display](#display)
  + [Recovery Phrase Entropy](#recovery-phrase-entropy)
  + [IOTA Security Concerns on Ledger Nano S](#iota-security-concerns-on-ledger-nano-s)
* [Future deprecation notes](#future-deprecation-notes)
* [How to get started](#how-to-get-started)
  + [Requirements](#requirements)
  + [Installation of the IOTA app on the ledger](#installation-of-the-iota-app-on-the-ledger)
* [FAQ](#faq)
    - [I lost my ledger, what should I do now?](#i-lost-my-ledger--what-should-i-do-now-)
    - [How can i uninstall the app?](#how-can-i-uninstall-the-app-)
* [Development](#development)
  + [Preparing development environment under Ubuntu 17.10](#preparing-development-environment-under-ubuntu-1710)
  + [Preparing development environment in other distributions](#preparing-development-environment-in-other-distributions)
  + [Compile and load the IOTA Ledger app](#compile-and-load-the-iota-ledger-app)
* [Documentation](#documentation)
* [Contributing](#contributing)
  + [Donations](#donations)
  + [As a developer](#as-a-developer)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>

---

## Introduction

IOTA is a unique cryptocurrency with specific design considerations that must be taken into account. This document will attempt to go over how the Ledger Nano S functions, and how to stay safe when using the Ledger Nano S for IOTA.

### Terminology

*Seed:* An IOTA seed is equivalent to the password for your "IOTA account". It can store a *near infinite* number of addresses.

*Private Key:* The private key is used to generate a public key. It is also used to prove you own said public key by means of creating a signature for a specific transaction.

*Public Key:* Also known as public address or just address. The public key is the key you would give to somebody else to transfer you funds.

*Change Tx:* After sending funds to a 3rd party, all remaining funds on the account must be transferred to a new address - this is called the change tx (or the change address).

*Account:* The IOTA app stores indexes for 5 seeds, these can be used as unique accounts. For example one could be a primary or general purpose account, and another could be a donations account.

### Address Reuse

IOTA uses a type of quantum resistance called the Winternitz One-Time Signature Scheme. This means that addresses **must not** be reused. This is because you reveal part of the private key with each transaction. As such, **reusing a spent address** can allow attackers to forge fake transactions and steal your balance.

Keep in mind you can **receive** an infinite number of transactions, but as soon as you **send** from the address, you must **NEVER** use that address again.

This is handled in IOTA by utilizing what is called a seed index. Each index in the seed generates a unique address. When creating an IOTA transaction, it uses what's called an *atomic bundle*. This is just a fancy term that means either the entire bundle is accepted, or none of it is.

### IOTA Bundle

A bundle is just a group of transactions and IOTA uses both input and output transactions. So if Bob has 10 iota, and wants to send Alice 3 iota, the bundle could look like this:

**tx1:** Bob -10 iota

**tx2:** Alice +3 iota

**tx3:** Bob +7 iota (change tx)

This example highlights how IOTA handles one time signatures. First it takes an input of 10 iota from Bob's address. It sends 3 of it to Alice, and it puts the remaining 7 iota on a new address belonging to Bob's seed.

All input transactions require the private key to generate a signature which proves that you are the owner of the funds.

Because bundles are *atomic units*, the network will never accept Bob's input tx of -10 iota without also accepting sending 3 to Alice, and 7 to a new address owned by Bob (in this example).

### Parts of an IOTA Transaction

An IOTA transaction is broken up into 2 halves. The first half is generating a transaction bundle, and creating signatures for it.

The second half is selecting 2 other transactions to confirm, and performing the proof of work.

The Ledger Nano S is **only** responsible for generating signatures for a specific transaction. After that the host machine (or anybody else for that matter), can take the signatures and broadcast it to the network (however the signatures are only valid for the specific transaction bundle).

**Promoting an unconfirmed transaction does not require re-signing on the Ledger.**

## How Ledger Nano S Works

The Ledger Nano S is a hardware wallet which deterministically generates an IOTA seed based on your 24 word mnemonic (created when setting up the Ledger Nano S).

Instead of storing the seed on your computer (which could be stolen by an attacker), the seed is stored on the Ledger Nano S, and is never broadcast to the host machine it is connected to.

Instead, the host machine must ask the Ledger to provide the information (such as public keys or signatures). When creating transactions the host will generate the necessary information for the transaction bundle, and then will send it to the Ledger Nano S to be signed. The Ledger will then use the private keys associated with the input transactions to generate unique signatures, and will then transfer **only the signatures** back to the host machine.

The host can then use these signatures (which are only valid for that specific transaction bundle) to broadcast the transaction to the network. However as you can see, neither the IOTA seed, nor any of the private keys ever leave the device. 

*However keep in mind that in IOTA the signature contains **some** information about the private key for one specific address.*

See [Ledger's documentation](http://ledger.readthedocs.io) to get more info about the inner workings of the Ledger Nano S.

## IOTA Specific Considerations on the Ledger Nano S

### IOTA User-Facing App Functions

#### Functions

- *Display Address:* The wallet can ask the Ledger to display the address for a specific index on the seed. It **only** accepts the index, and subsequently generates the address itself and thus verifies that the address belongs to the Ledger.

    *Note: this does not pertain to displaying addresses in transactions. Transactions will be denoted as "output", "input", or "change" transactions. Input and Change are verified to belong to the Ledger. Output are not.*

- *Initialize Ledger Indexes:* The wallet will provide new indexes to write to the Ledger Nano S. **THIS IS A CRITICAL OPERATION.** If you approve inaccurate/bad indexes from the wallet you could generate bad transactions and lose funds.

    If this happens you should use the wallet to reinitialize the seed from scratch. This could potentially take a long time as the Ledger hardware is slow at generating addresses (compared to a computer). The wallet has to ask the Ledger for new addresses until the last address was not used in the tangle yet (not spend from and no funds on it).

- *Sign Transaction:* The wallet will generate a transaction for the user to approve before the Ledger signs it. **Ensure all amounts and addresses are correct before signing**. These signatures are then sent back to the host machine.

#### Display

- The sides of the display will have an up or down arrow indicating that you can scroll to a new screen.

- Two bars along the top (just below the buttons) indicates that there is a double press function available (generally confirm/toggle). We will be working to ensure this function is always intuitive.

### Recovery Phrase Entropy

- The 24 word BIP39 recovery phrase (mnemonic) of the Ledger Nano S represents less information (256 bits of entropy) than a 27 tryte IOTA seed (384 bits of entropy). 
- An additional function is used to convert this mnemonic seed and the optional passphrase (not your pin number) of your choosing into a 512 bit binary seed. This happens according to the BIP39 standard on the Ledger Nano using system calls.
- The extended child key (512 bits) of a corresponding BIP32 path is then hashed (using Kerl) to derive the final 243 trit seed for IOTA.

While having (only) 256 bits of entropy does not pose a security problem, it does not support the full potential of the IOTA seed. Thus, to use the full entropy supported by IOTA, an additional sufficiently long passphrase is needed! On the other hand, there are other factors that might have a higher security impact, like choosing proper random mnemonics (the Ledger Nano uses a TRNG for that matter) or selecting a higher security level.

### IOTA Security Concerns on Ledger Nano S

All warnings on the Ledger are there for a reason, **MAKE SURE TO READ THEM** and refer to this document if there is any confusion.

- **The IOTA Ledger app is stateful**

    What this means is that the address index for your IOTA seed is stored in flash memory on the Ledger device. Therefore uninstalling the IOTA app will erase these indexes.
    
    Don't worry! There is no security risk by losing your address indexes and if the IOTA wallet you are using (Ex. Trinity or Romeo) has their own cache of the addresses used, you shouldn't notice any issues.
    
    However it is still recommended to back up your address indexes before deleting the app. (Either by hand through the options menu, or automatically using a wallet - currently not supported).

- **Ensure the change index is always increasing.**

    The Ledger generates 5 unique IOTA seeds based on the 24 word mnemonic, and tracks the index for each seed. Because the index is stored on the device, you can quickly transfer to a new wallet and initialize your seed faster than scanning every index to find the one that holds your funds.

    Whenever you sign a transaction with the Ledger it will write whatever the seed-index of the change transaction is into memory on the Ledger.

    As such it is *strongly recommended* to never use a change index that is lower than an address that has already been used, as this can inadvertently cause address reuse.

    If a Ledger has a seed-index of 10 in memory, and a new transaction uses the change index of any value lower than 10, it will warn the user and ask "Are you sure?".

- **Don't rush through signing a transaction.**

    When generating a transaction for the Ledger to sign, you will scroll through each transaction before signing off on the bundle. The transaction information will come up in order (while scrolling from left to right). The first screen will display the tx type (output, input, or change), as well as the amount. The next screen will display the corresponding address for said transaction. This will repeat until all transactions have been displayed, then you can select "Approve" or "Deny".

    - All output transactions to 3rd party addresses will say "Output:" and below that "1.56 Mi" (for example). "Output" being the key word here.

        All input transactions will say "Input:", and the final output transaction (the change transaction) will say "Change: [4]" ([4] being the seed-index of the change tx). This means the Ledger has verified the address used for the change tx belongs to the Ledger.

        **If you are not sending all of your funds off of the seed, MAKE SURE one of the tx says "Change".**

        *Note: When transferring between IOTA Ledger accounts (from one account seed to another) the ledger will not display it as a "Change tx". The wallet should first display the address that it intends to send it to on the Ledger (thus verifying it still belongs to the Ledger), and then create the transaction. The user would then verify in the transaction that the "Output:" tx is in fact going to the address previously displayed on the Ledger.*

- **Verify ALL information in a transaction and NEVER re-sign a transaction.**

    For other coins you only need to verify the destination address and amount, but for IOTA you must also verify all input transactions. If you sign a transaction on the Ledger but the transaction is not using all of the funds on a specific address, leftover funds on the address are then vulnerable.

    As such make sure the input amount(s) are what you expect them to be, make sure the output destination and amount is what you intend to send, and make sure there is a change tx if applicable with all of your remaining funds.

    *Note: After signing a transaction, you should verify the transaction was successfully broadcast to the network. You should **NEVER** re-sign the same transaction on the Ledger. If the transaction does not get confirmed, you should promote the transaction **on the host machine**. This avoids generating a new signature and weakening the security of the addresses.*

    - If the transaction was not broadcast to the network, and you can't find it in the wallet you should be **EXTREMELY CAUTIOUS** before proceeding to sign a new transaction. If an infected machine is silently storing the signatures without broadcasting them, it could steal your funds after re-signing.

        If this situation should arise you should consider going to a more trusted machine before re-signing a transaction.

## Future deprecation notes

- **Address generation**

    In the current alpha version of the IOTA application on Ledger we use a special test path for the address generator that might be incompatible with future versions. This is due to the fact that Romeo web wallet uses a different approach than Trinity to keep track of addresses (paging system vs cache). It could happen that when we integrate the Ledger in Trinity, you end up with **a different seed and therefore addresses than you had before**.

    In such an event, you will have to move your funds over from the existing addresses to a new address. This may include that you must send your funds off the Ledger to an IOTA desktop wallet like Trinity, then update your IOTA ledger app, and transfer your funds back to the new ledger addresses.

    It is highly recommended that you do not send all your funds to the ledger as long as this is heavily in alpha!

## How to get started

### Requirements

- Make sure that your Ledger Nano S is running firmware 1.4.2.
For update instructions see: [How to update my Ledger Nano S with the firmware 1.4](https://support.ledgerwallet.com/hc/en-us/articles/360001340473-How-to-update-my-Ledger-Nano-S-with-the-firmware-1-4)

### Installation of the IOTA app on the ledger

You can follow one of these methods to install the IOTA app to your Ledger device:

- If you would like a one-size-fits-all (including Windows, Mac, Linux) solution you can find instructions on how to install the app using VirtualBox here: [blue-app-iota-loader-alpine](https://github.com/IOTA-Ledger/blue-app-iota-loader-alpine)
    This option will download a pre-configured Virtual Machine with the necessary prerequisites to install the app.
    The application is automatically downloaded from our [releases page](https://github.com/IOTA-Ledger/blue-app-iota/releases), checked for correct digital signature and then installed.

- If you don't like VirtualBox, and are on a Mac or Linux machine you can find instructions to install the app here: [blue-app-iota-loader](https://github.com/IOTA-Ledger/blue-app-iota-loader)
- If you are a developer, just install the toolchain described here: [Development](#development)

## FAQ

#### I lost my ledger, what should I do now?

Hopefully you wrote down your 24 recovery words and your optional passphrase in a safe place. If not, all your funds are lost.

If you did, the best solution is to buy a new Ledger Nano S and enter your 24 recovery words and your optional passphrase in the new device.<br>
After installation of the IOTA Ledger app, all your funds are restored. Take care to reinitialize your seed index correctly.

Another approach is to use our seed recovery tool which can be found here: https://github.com/IOTA-Ledger/recover-iota-seed-from-ledger-mnemonics.<br>
**WARNING: Only use this tool in emergencies**, as exposing your seed defeats the primary purpose of the Ledger Nano S.

#### How can i uninstall the app?

In order to uninstall the app simply run the installation of the app again, and this time it will first prompt you remove the existing app. 
Once the existing app is removed, just end the process (Don’t approve re-installing it).

## Development

### Preparing development environment under Ubuntu 17.10

- Clone this repo
- Execute the following commands to setup your development environment:
```
cd blue-app-iota
chmod +x install_dev_env.sh
chmod +x activate_virt_env.sh
./install_dev_env.sh
```
- If you execute it for the first time, maybe you have to log out and log in again to get correct group rights

### Preparing development environment in other distributions

- Clone this repo, and set up your development environment according to this: [LedgerHQ Getting Started](https://github.com/LedgerHQ/ledger-dev-doc/blob/master/source/userspace/getting_started.rst)
- ATTENTION: Ledger Python 2 library seems to be broken, so you have to use Python 3 and the latest version of the ledger Python lib from their [GitHub](https://github.com/LedgerHQ/blue-loader-python). Have a look here:

> I was unable to install the recent update of the Ledger SDK with their direct instructions. Instead, follow everything from the link above, except for the Python SDK.
> Instead of running Python 2 like explicitly being told, use Python 3 instead (I wasn't able to make it work with Python 2 and the commits of their latest updates over the past week suggest they created Python 3 support).
> In the part where they tell you to run `pip install ledgerblue`, run `pip install git+https://github.com/LedgerHQ/blue-loader-python.git` to install the bleeding-edge version.

### Compile and load the IOTA Ledger app

- Connect your Ledger to the PC and unlock it
- To load the app, be sure that the dashboard is opened in the Ledger
- Run the following commands to compile the app from source and load it
```
cd blue-app-iota

# If you have installed using the automated script:
./activate_virt_env.sh

make load
```
- Accept all the messages on the Ledger
- You can now make your first IOTA Ledger address by running the following command (while Ledger is plugged in and the IOTA App is opened):
```
python demos/python/demo.py
```

## Documentation

See: [APDU API Documentation](/APDU_API.md)

## Contributing

### Donations
Would you like to donate to help the development team? Send some IOTA to the following address:
```
J9KPGBTWIKTRBIWXNDCZUWWWVVESYVISFJIY9GCMGVLQXFJBDAKLLN9PNAZOOUZFZDGDSFPWCTJYILDF9WOEVDQVMY
```
Please know that the donations made to this address will be shared with everyone who contributes (the contributions has to be worth something, of course)

### As a developer
Would you like to contribute as a dev? Please check out our [Discord channel](https://discord.gg/U3qRjZj) to contact us!
