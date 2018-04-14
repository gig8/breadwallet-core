/*
 * BreadWallet
 *
 * Created by Ed Gamble <ed@breadwallet.com> on 1/22/18.
 * Copyright (c) 2018 breadwallet LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package com.breadwallet.core;

import java.lang.ref.WeakReference;

/**
 *
 */
public class BRCoreWallet extends BRCoreJniReference
{
    public interface Listener {
        // func balanceChanged(_ balance: UInt64)
        void balanceChanged(long balance);

        // func txAdded(_ tx: BRTxRef)
        void onTxAdded(BRCoreTransaction transaction);

        // func txUpdated(_ txHashes: [UInt256], blockHeight: UInt32, timestamp: UInt32)
        void onTxUpdated(String hash, int blockHeight, int timeStamp);

        // func txDeleted(_ txHash: UInt256, notifyUser: Bool, recommendRescan: Bool)
        void onTxDeleted(String hash, int notifyUser, final int recommendRescan);
    }

    //
    // Hold a weak reference to the listener.  It is a weak reference because it is likely to
    // be self-referential which would prevent GC of this Wallet.  This listener is used
    // by the BRCore Wallet, and specifically in a BRCore thread context to invoke the
    // Listener methods.  Because of the use in a BRCore thread, the Listener *MUST BE* a
    // JNI Global Ref.  The installListener() method, called in the BRCoreWallet constructor
    // initializes 'listener' with a GlobalWeakReference.
    //
    protected WeakReference<Listener> listener = null;

    //
    //
    //
    public BRCoreWallet(BRCoreTransaction[] transactions,
                        BRCoreMasterPubKey masterPubKey,
                        int forkId,
                        Listener listener)
    {
        super (createJniCoreWallet(transactions, masterPubKey, forkId));
        assert (null != listener);
        this.listener = new WeakReference<>(listener);

        installListener (listener);

        // All `transactions` are effectively registered - now 'owned' by wallet
        for (BRCoreTransaction transaction : transactions)
            transaction.isRegistered = true;
    }

    protected static native long createJniCoreWallet (BRCoreTransaction[] transactions,
                                                      BRCoreMasterPubKey masterPubKey,
                                                      int forkId);

    protected native void installListener (Listener listener);

    // returns the first unused external address
    // BRAddress BRWalletReceiveAddress(BRWallet *wallet);
    public native BRCoreAddress getReceiveAddress ();

    // writes all addresses previously genereated with BRWalletUnusedAddrs() to addrs
    // returns the number addresses written, or total number available if addrs is NULL
    public native BRCoreAddress[] getAllAddresses ();

    // true if the address was previously generated by BRWalletUnusedAddrs() (even if it's now used)
    // int BRWalletContainsAddress(BRWallet *wallet, const char *addr);
    public native boolean containsAddress (BRCoreAddress address);

    // true if the address was previously used as an input or output in any wallet transaction
    // int BRWalletAddressIsUsed(BRWallet *wallet, const char *addr);
    public native boolean addressIsUsed (BRCoreAddress address);

    // TODO: Holding these transactions when the wallet is GCed.... boom!?
    public BRCoreTransaction[] getTransactions () {
        BRCoreTransaction[] transactions = jniGetTransactions();

        // Mark as 'registered' if not a copy.
        for (BRCoreTransaction transaction : transactions) {
            assert (transaction.isSigned());
            transaction.isRegistered = !BRCoreTransaction.JNI_COPIES_TRANSACTIONS;
        }

        return transactions;
    }

    /**
     * Return *registered* transactions in wallet - these are not copies of the JNI C transaction
     * Therefore they *must* be marked 'isRegistered = true'
     * @return
     */
    private native BRCoreTransaction[] jniGetTransactions ();

    public native BRCoreTransaction[] getTransactionsConfirmedBefore (long blockHeight);

    public native long getBalance ();

    public native long getTotalSent ();

    public native long getTotalReceived ();

    // UTXOs

    public native long getFeePerKb ();

    public native void setFeePerKb (long feePerKb);

    public native long getMaxFeePerKb ();

    public native long getDefaultFeePerKb ();

    /**
     * Creates a BRCoreTransaction for sending `amount` to `address`.  Will create a
     * BRCoreTransactionOutput for `address` with a script of:
     *      DUP HASH160 <pub key hash for address> EQUALVERIFY CHECKSIG
     * (provided `address` is a 'pub key address'; otherwise produces a script for a scriptAddress)
     *
     * Will iterate over the wallet's UTXOs adding in their transaction output (amount, script) as
     * a BRCoreTransactionInput.  If the UTXOs can't cover `amount` then `null` is returned;
     * otherwise a 'change' output is added for an unused wallet address.
     *
     * @param amount the amount to send
     * @param address the address to send to
     * @return a consistently constructed transaction.
     */
    public native BRCoreTransaction createTransaction (long amount, BRCoreAddress address);

    /**
     * Create a BRCoreTransaction with the provided outputs
     *
     * @param outputs the outputs to include
     * @return a consistently constructed transaction (input selected, fees handled, etc)
     */
    public native BRCoreTransaction createTransactionForOutputs (BRCoreTransactionOutput[] outputs);

    // Need to remove 'forkId' - should be derived from the chainParams leading to this wallet.

    /**
     * Sign `transaction` for the provided `forkId` (BTC or BCH) using `phrase`.  The `phrase` must
     * be the 'paper key' used when the wallet's MasterPubKey was originally created.
     *
     * @param transaction
     * @param phrase
     * @return
     */
    public native boolean signTransaction (BRCoreTransaction transaction, byte[] phrase);

    public native boolean containsTransaction (BRCoreTransaction transaction);

    public boolean registerTransaction (BRCoreTransaction transaction) {
        // Try to register; this might fail on a 're-register' attempt.
        boolean registered = jniRegisterTransaction(transaction);

        // Make isRegistered sticky.
        transaction.isRegistered = transaction.isRegistered || registered;

        // Return the status of the register.
        return registered;
    }

    private native boolean jniRegisterTransaction (BRCoreTransaction transaction);

    public native void removeTransaction (byte[] transactionHash);

    public native void updateTransactions (byte[][] transactionsHashes, long blockHeight, long timestamp);

    /**
     * Returns the BRCoreTransaction with the provided `transactionHash` if it exists; otherwise
     * NULL is returned.
     *
     * If BRCoreTransaction.JNI_COPIES_TRANSACTIONS is true, then the returned transaction is
     * a copy of the Core BRTransaction - TBD caution when passed back into Core
     *
     * @param transactionHash
     * @return
     */
    public BRCoreTransaction transactionForHash (byte[] transactionHash) {
        BRCoreTransaction transaction = jniTransactionForHash(transactionHash);

        // We mark as 'registered' if not a copy.
        if (null != transaction)
            transaction.isRegistered = transaction.isRegistered
                || !BRCoreTransaction.JNI_COPIES_TRANSACTIONS;

        return transaction;
    }

    private native BRCoreTransaction jniTransactionForHash (byte[] transactionHash);

    /**
     * Check if a transaction is valid - THIS METHOD WILL FATAL if the transaction is not signed.
     * You must call transaction.isSigned to avoid the FATAL.
     *
     * @param transaction
     * @return
     */
    public native boolean transactionIsValid (BRCoreTransaction transaction);

    public native boolean transactionIsPending (BRCoreTransaction transaction);

    public native boolean transactionIsVerified (BRCoreTransaction transaction);


    // set TX Unconfirmed After


    /**
     * Return the net amount received by this wallet.  If the amount is positive, then the balance
     * of this wallet increased; if the amount is negative, then the balance decreased.
     *
     * @param tx
     * @return
     */
    public long getTransactionAmount (BRCoreTransaction tx)
    {
        long amountSent = getTransactionAmountSent(tx);
        long amountReceived = getTransactionAmountReceived(tx);

        return amountSent == 0
                ? amountReceived
                : -1 * (amountSent - amountReceived - getTransactionFee(tx));
    }

    public native long getTransactionFee (BRCoreTransaction tx);

    public native long getTransactionAmountSent (BRCoreTransaction tx);

    public native long getTransactionAmountReceived (BRCoreTransaction tx);

    public native long getBalanceAfterTransaction (BRCoreTransaction transaction);

    /**
     * Return a BRCoreAddress for a) the receiver (if we sent an amount) or b) the sender (if
     * we received an amount).  The returned address will be the first address that is not in
     * this wallet from the outputs or the inputs, respectively.
     *
     * @param transaction
     * @return
     */
    public BRCoreAddress getTransactionAddress (BRCoreTransaction transaction){
        return getTransactionAmount(transaction) > 0
                ? getTransactionAddressInputs(transaction)   // we received -> from inputs
                : getTransactionAddressOutputs(transaction); // we sent     -> to outputs
    }

    /**
     * Return the first BRCoreAddress from the `transaction` inputs that is not an address
     * in this wallet.
     *
     * @param transaction
     * @return The/A BRCoreAddress that received an amount from us (that we sent to)
     */
    public BRCoreAddress getTransactionAddressInputs(BRCoreTransaction transaction) {
        for (BRCoreTransactionInput input : transaction.getInputs()) {
            BRCoreAddress address = new BRCoreAddress(input.getAddress());
            if (!containsAddress(address))
                return address;
        }
        return null;
    }

    /**
     * Return the first BRCoreAddress from the `transaction` outputs this is not an address
     * in this wallet.
     *
     * @param transaction
     * @return The/A BRCoreAddress that sent to us.
     */
    public BRCoreAddress getTransactionAddressOutputs(BRCoreTransaction transaction) {
        for (BRCoreTransactionOutput output : transaction.getOutputs()) {
            BRCoreAddress address = new BRCoreAddress (output.getAddress());
            if (!containsAddress (address))
                return address;
        }
        return null;
    }

    public native long getFeeForTransactionSize (long size);

    public native long getFeeForTransactionAmount (long amount);

    public native long getMinOutputAmount ();

    public native long getMaxOutputAmount ();

    public native int getForkId ();

    // local amount

    // bitcoin amount

    // To call BRWalletFree()
    public native void disposeNative ();

    protected static native void initializeNative ();

    static { initializeNative(); }
}

